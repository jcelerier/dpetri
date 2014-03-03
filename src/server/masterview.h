#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include <QListWidgetItem>
#include <client/localclient.h>

#include <clientmanager.h>
#include "serverlogic.h"
namespace Ui {
	class MasterView;
}
class MainWindow;
class MasterView : public QWidget
{
		friend class MainWindow;
		Q_OBJECT
	public:
		explicit MasterView(QWidget *parent = 0);
		~MasterView();
		void setServerLogic(ServerLogic* pnm);
	signals:
		void play();

	public slots:
		void updateConnectionList();
		void updateClientPool(int id);
		void updateLocalPool();
		void onClientSelection(QListWidgetItem*);

		void updateNet();

		void addLog(QString s);
	private:
		ServerLogic* _logic;
		Ui::MasterView *ui;
};

#endif // MASTERVIEW_H
