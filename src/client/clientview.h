#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QWidget>
#include "clientlogic.h"

namespace Ui {
	class ClientView;
}
class MainWindow;
class ClientView : public QWidget
{
		Q_OBJECT

	public:
		explicit ClientView(QWidget *parent = 0);
		~ClientView();

		void setClientLogic(ClientLogic*);

	signals:
		void take(QString);
		void give(QString);

	public slots:
		void updateNet();
		void updateServerPool();
		void updateLocalPool();

	private slots:
		void clientToPool();
		void poolToClient();

	private:
		Ui::ClientView *ui;
		LocalClient* _client;
		ClientLogic* _logic;
};

#endif // CLIENTVIEW_H
