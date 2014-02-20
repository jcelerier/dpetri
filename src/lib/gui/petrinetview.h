#pragma once

#include <QtWidgets/QGridLayout>
#include <QtSvg/QSvgWidget>
#include "petrinetmodel.h"
#include "../petrinettools.h"

class PetriNetView : public QWidget
{
		Q_OBJECT
	public:
		explicit PetriNetView(QWidget *parent = 0) :
			QWidget(parent)
		{
			mainLayout->addWidget(viewer, 0, 0, 15, 15);
			setLayout(mainLayout);
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
			PetriNetSerializer ser(*net);
			const char * cstr = ser.toSVG();

			QByteArray qdata(cstr, ser.size());
			viewer->load(qdata);
			mainLayout->removeWidget(viewer);
			mainLayout->addWidget(viewer);
			setLayout(mainLayout);
		}

	private:
		PetriNet* net;
		QSvgWidget *viewer{new QSvgWidget};
		QGridLayout *mainLayout{new QGridLayout};
};
