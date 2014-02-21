#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QWidget>
#include <gui/petrinetmodel.h>

namespace Ui {
	class ClientView;
}

class ClientView : public QWidget
{
		Q_OBJECT

	public:
		explicit ClientView(QWidget *parent = 0);
		~ClientView();

		void setPetriNetModel(PetriNetModel& pnm);


	signals:
		void take(QString);

	public slots:
		void updatePool();
	private slots:
		void clientToPool();
		void poolToClient();

	private:
		Ui::ClientView *ui;
		PetriNetModel* model;
};

#endif // CLIENTVIEW_H
