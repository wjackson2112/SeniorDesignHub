#ifndef __HubList_h__
#define __HubList_h__

#include "ui_HubList.h"

class AddHub;
class HubListModel;

class HubList : public QMainWindow
{
	Q_OBJECT

public:
	HubList();

protected slots:
	void RemoveHub();
	void ShowConfig();
	void doubleClicked(const QModelIndex& index);
	void selectionChanged(const QItemSelection& selected,
		const QItemSelection& deselected);

private:
	Ui::HubList ui;

	HubListModel *mModel;
	AddHub *mAddDialog;
};

#endif // __HubList_h__
