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


	private:
		Ui::ClientView *ui;
};

#endif // CLIENTVIEW_H
