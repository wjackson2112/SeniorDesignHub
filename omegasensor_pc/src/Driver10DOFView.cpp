#include "Driver10DOFView.h"
#include "Driver10DOFModel.h"

Driver10DOFView::Driver10DOFView(QWidget *p) : QWidget(p), mModel(0)
{
	// Create the GUI.
	ui.setupUi(this);
}

void Driver10DOFView::SetModel(Driver10DOFModel *model)
{
	// Delete the old model.
	if(mModel)
		delete mModel;

	mModel = model;
	Q_ASSERT(mModel);

	connect(mModel, SIGNAL(NewAccel(float, float, float)),
		this, SLOT(Update()));
}

void Driver10DOFView::SetDeviceName(const QString& name)
{
	mDeviceName = name;
	setWindowTitle(tr("10DOF Sensor Module - %1").arg(name));
}

void Driver10DOFView::Update()
{
	QList<float> accelX, accelY, accelZ;

	for(int i = mModel->NumAccelValues() - 1; i >= 0; i--)
	{
		accelX.append(mModel->AccelX(i));
		accelY.append(mModel->AccelY(i));
		accelZ.append(mModel->AccelZ(i));
	}

	ui.accelXPlot->SetRange(-2.0f, 2.0f);
	ui.accelXPlot->SetData(accelX);

	ui.accelYPlot->SetRange(-2.0f, 2.0f);
	ui.accelYPlot->SetData(accelY);

	ui.accelZPlot->SetRange(-2.0f, 2.0f);
	ui.accelZPlot->SetData(accelZ);

	ui.accelXLabel->setText(tr("X: %1g").arg(
		mModel->AccelX(0)));

	ui.accelYLabel->setText(tr("Y: %1g").arg(
		mModel->AccelY(0)));

	ui.accelZLabel->setText(tr("Z: %1g").arg(
		mModel->AccelZ(0)));

	float accelMag = sqrt(mModel->AccelX(0) * mModel->AccelX(0) +
		mModel->AccelY(0) * mModel->AccelY(0) +
		mModel->AccelZ(0) * mModel->AccelZ(0));

	ui.accelMagLabel->setText(tr("Magnitude: %1g").arg(
		accelMag));
}
