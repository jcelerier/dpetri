#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QWidget>
#include <gui/petrinetmodel.h>

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

		void setPetriNetModel(PetriNetModel& pnm);
		void setParent(MainWindow* window);

	signals:
		void take(QString);

	public slots:
		void updatePool();
		void updateLocalPool();
	private slots:
		void clientToPool();
		void poolToClient();

	private:
		Ui::ClientView *ui;
		PetriNetModel* model;
		MainWindow* myParent;
};

#endif // CLIENTVIEW_H
