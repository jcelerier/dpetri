#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include <QListWidgetItem>
#include <gui/petrinetmodel.h>
#include <clientmanager.h>
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
		void setOscManager(ClientManager& osc);

	signals:
		void play();
	public slots:
		void updateConnectionList();
		void updateClientPool(int id);
		void updateLocalPool();
		void onClientSelection(QListWidgetItem*);
	private:
		PetriNetModel* model;
		Ui::MasterView *ui;

		ClientManager* pOscmgr;
};

#endif // MASTERVIEW_H
