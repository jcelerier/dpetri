#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>

#include <graphviz/types.h>
#include <graphviz/graph.h>
#include <graphviz/cdt.h>
#include <graphviz/gvc.h>

#include <QtWidgets/QGridLayout>
#include <QtSvg/QSvgWidget>
#include <QDebug>
#include "petrinetmodel.h"

class PetriNetView : public QWidget
{
		Q_OBJECT
	public:
		explicit PetriNetView(QWidget *parent = 0) :
			QWidget(parent)
		{
		}
		void setModel(PetriNetModel& p)
		{
			connect(&p, SIGNAL(changed()), this, SLOT(updatePetriNet()));
			net = &p.net;
		}
	signals:

	public slots:
		void updatePetriNet()
		{
			//// Create DOT
			std::stringstream output;
			output << io::dot << *net;

			// For weird reasons c_str does not work here.
			auto str = output.str();
			char * cstr = new char[str.length() + 1];
			std::copy(str.begin(), str.end(), cstr);
			cstr[str.length()] = 0;

			//// Create SVG from DOT
			GVC_t* gvc = gvContext();
			Agraph_t* G = agmemread(cstr);

			agset(G, (char*) "label", (char*)"");
			gvLayout (gvc, G, "neato");

			char* renderedData;
			unsigned int renderedLength;
			gvRenderData(gvc, G, "svg", &renderedData, &renderedLength);
			gvFreeLayout(gvc, G);

			agclose (G);

			gvFreeContext(gvc);

			//// Display SVG
			QByteArray qdata(renderedData, renderedLength);
			QSvgWidget *viewer = new QSvgWidget();
			viewer->load(qdata);

			QGridLayout *mainLayout = new QGridLayout;
			mainLayout->addWidget(viewer, 0, 0, 0, 0);
			setLayout(mainLayout);
		}

	private:
		PetriNet* net;
};
