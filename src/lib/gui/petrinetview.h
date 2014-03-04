#pragma once

#ifdef USE_PNAPI
#include <QtWidgets/QHBoxLayout>
#include <QtSvg/QSvgWidget>
#include "petrinetmodel.h"
#include "../petrinetserializer.h"
#include <QSpacerItem>
#include <pnapi/pnapi.h>

class PetriNetView : public QWidget
{
		Q_OBJECT
	public:
		explicit PetriNetView(QWidget *parent = 0) :
			QWidget(parent)
		{
			mainLayout->addWidget(viewer);
			setLayout(mainLayout);
		}

	public slots:
		void updatePetriNet(const pnapi::PetriNet& net)
		{
			PetriNetSerializer<pnapi::PetriNet> ser(net);
			const char * cstr = ser.toSVG();

			QByteArray qdata(cstr, ser.size());
			viewer->load(qdata);
			mainLayout->removeWidget(viewer);
			mainLayout->addWidget(viewer);
			setLayout(mainLayout);
		}

	private:
		QSvgWidget *viewer{new QSvgWidget};
		QHBoxLayout *mainLayout{new QHBoxLayout};
};
#endif
