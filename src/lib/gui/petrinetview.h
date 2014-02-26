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
			connect(&p, SIGNAL(netChanged()), this, SLOT(updatePetriNet()));
			model = &p;
		}
	signals:

	public slots:

		void updatePetriNet()
		{
			PetriNetSerializer ser(model->net());
			const char * cstr = ser.toSVG();

			QByteArray qdata(cstr, ser.size());
			viewer->load(qdata);
			mainLayout->removeWidget(viewer);
			mainLayout->addWidget(viewer);
			setLayout(mainLayout);
		}

	private:
		PetriNetModel* model;
		QSvgWidget *viewer{new QSvgWidget};
		QGridLayout *mainLayout{new QGridLayout};
};
