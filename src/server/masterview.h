#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include <gui/petrinetmodel.h>
namespace Ui {
	class MasterView;
}

class MasterView : public QWidget
{
		Q_OBJECT
	public:
		explicit MasterView(QWidget *parent = 0);
		~MasterView();
		void setPetriNetModel(PetriNetModel& pnm);

	public slots:
		void updateConnectionList();
	private:
		Ui::MasterView *ui;
};

#endif // MASTERVIEW_H
