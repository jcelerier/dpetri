#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <QMainWindow>
#include "androidclientlogic.h"
namespace Ui {
	class MainApplication;
}

class MainApplication : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainApplication(QWidget *parent = 0);
		~MainApplication();

	private:
		Ui::MainApplication *ui;
		AndroidClientLogic logic;
};

#endif // MAINAPPLICATION_H
