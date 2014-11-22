#ifndef __AddHub_h__
#define __AddHub_h__

#include "ui_AddHub.h"

class HubListModel;

class AddHub : public QDialog
{
	Q_OBJECT

public:
	AddHub(QWidget *parent = 0);
	~AddHub();

protected:
	virtual void showEvent(QShowEvent *evt);

protected slots:
	void AddClicked();
	void doubleClicked(const QModelIndex& index);
	void selectionChanged(const QItemSelection& selected,
		const QItemSelection& deselected);

private:
	Ui::AddHub ui;

	HubListModel *mModel;
};

#endif // __AddHub_h__
