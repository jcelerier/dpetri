#pragma once
#include <pnapi/pnapi.h>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace pnapi;
class PetriNetModel : public QObject
{
		Q_OBJECT
	public:
		PetriNetModel(QObject * parent):
			QObject(parent)
		{

		}

		PetriNet net;

	public slots:
		void loadFile()
		{
			QString file = QFileDialog::getOpenFileName(nullptr,
														tr("Choose a PNML file"),
														QString(),
														tr("PNML File (*.pnml)"));

			std::ifstream in (file.toStdString());
			in >> io::pnml >> net;

			emit changed();
		}

		void loadFromString(std::string str)
		{
			std::stringstream s(str);
			s >> io::pnml >> net;

			emit changed();
		}

	signals:
		void changed();
};
