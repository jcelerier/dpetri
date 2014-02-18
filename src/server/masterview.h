#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>

namespace Ui {
	class MasterView;
}

class MasterView : public QWidget
{
		Q_OBJECT

	public:
		explicit MasterView(QWidget *parent = 0);
		~MasterView();

	private:
		Ui::MasterView *ui;
};

#endif // MASTERVIEW_H
