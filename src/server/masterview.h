#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include <gui/petrinetmodel.h>
#include <oscconnectionmanager.h>
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
		void setPetriNetModel(PetriNetModel& pnm);
		void setOscManager(OSCConnectionManager& osc);

	signals:
		void play();
	public slots:
		void updateConnectionList();
	private:
		Ui::MasterView *ui;

		OSCConnectionManager* pOscmgr;
};

#endif // MASTERVIEW_H
