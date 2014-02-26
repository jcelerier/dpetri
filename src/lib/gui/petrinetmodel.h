#pragma once
#include <pnapi/pnapi.h>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../clock.h"
#include "../nodepool.h"

#include "executionalgorithmbase.h"

using namespace pnapi;
class PetriNetModel : public QObject
{
		Q_OBJECT
	private:
		LocalExecutionAlgorithm _algorithm;
		PetriNet _net;
		NodePool _pool;
		Clock _clock;

	public:
		PetriNetModel(QObject * parent):
			QObject(parent),
			_algorithm(_net, _clock)
		{
		}

		const PetriNet& net() const
		{
			return _net;
		}

		NodePool& pool()
		{
			return _pool;
		}

	public slots:
		void start()
		{
			_clock.start();
			_algorithm.start();
		}

		void loadFile()
		{
			QString file = QFileDialog::getOpenFileName(nullptr,
														tr("Choose a Petri Net"),
														QString(),
														tr("FIONA File (*.fiona)"));

			std::ifstream in (file.toStdString());
			in >> io::owfn >> _net;

			emit netChanged();
			_pool.reload(_net);
			emit poolChanged();
		}

		void loadFromString(std::string str)
		{
			std::stringstream s(str);
			s >> io::owfn >> _net;

			emit netChanged();
		}

	signals:
		void netChanged();
		void poolChanged();
};
