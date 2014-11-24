#include "DriverDemoView.h"
#include "DriverDemoModel.h"

DriverDemoView::DriverDemoView(QWidget *p) : QWidget(p), mModel(0)
{
	// Create the GUI.
	ui.setupUi(this);
}

void DriverDemoView::SetModel(DriverDemoModel *model)
{
	// Delete the old model.
	if(mModel)
		delete mModel;

	mModel = model;
	Q_ASSERT(mModel);

	connect(ui.offButton, SIGNAL(clicked(bool)), mModel, SLOT(LedOff()));
	connect(ui.redButton, SIGNAL(clicked(bool)), mModel, SLOT(LedRed()));
	connect(ui.greenButton, SIGNAL(clicked(bool)), mModel, SLOT(LedGreen()));
	connect(ui.blueButton, SIGNAL(clicked(bool)), mModel, SLOT(LedBlue()));
	connect(ui.minButton, SIGNAL(clicked(bool)), this, SLOT(SetMin()));
	connect(ui.maxButton, SIGNAL(clicked(bool)), this, SLOT(SetMax()));
	connect(ui.resetButton, SIGNAL(clicked(bool)), this, SLOT(Reset()));
	connect(mModel, SIGNAL(NewValue(int)), this, SLOT(Update()));
}

void DriverDemoView::SetDeviceName(const QString& name)
{
	mDeviceName = name;
	setWindowTitle(tr("Demo Driver - %1").arg(name));
}
void DriverDemoView::SetMin()
{
	mModel->SetMinimum(mModel->Value());
	Update();
}

void DriverDemoView::SetMax()
{
	mModel->SetMaximum(mModel->Value());
	Update();
}

void DriverDemoView::Reset()
{
	mModel->SetMinimum(0);
	mModel->SetMaximum(1023);
	Update();
}

void DriverDemoView::Update()
{
	QList<float> data;

	for(int i = mModel->NumValues() - 1; i >= 0; i--)
		data.append(mModel->Value(i));

	ui.plot->SetRange(mModel->Minimum(), mModel->Maximum());
	ui.plot->SetData(data);

	ui.valueLabel->setText(tr("Value: %1 (%2)").arg(\
		mModel->Value()).arg(mModel->RawValue()));
	ui.rangeLabel->setText(tr("Range: [%1, %2]").arg(\
		mModel->Minimum()).arg(mModel->Maximum()));
	ui.adcValue->setMinimum(mModel->Minimum());
	ui.adcValue->setMaximum(mModel->Maximum());
	ui.adcValue->setValue(mModel->Value());
}
