#include "medicalsystem.h"

MedicalSystem::MedicalSystem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initWindow();
}

void MedicalSystem::initWindow(void)
{
	timer = new QTimer(this);
	model = new QSqlTableModel(this);
	model_d = new QSqlTableModel(this);
	model_p = new QSqlTableModel(this);
		
	timer->setInterval(1000);				//设置定时器触发间隔
	timer->start();							//设置定时器初始值
		
	model->setTable("basic_inf");			//基本信息视图
	model->select();
	model_d->setTable("detail_inf");		//附加详细信息视图
	model_d->select();
	model_p->setTable("patient_inf");
	model_p->select();
	
	connect(timer, SIGNAL(timeout()), this, SLOT(on_timeUpdate()));
	ui.doctorTableView->setModel(model);	//数据网格信息加载

	ui.doctorTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.timeTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.timeTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MedicalSystem::showDoctorPhoto(void)
{
	QPixmap photo;
	QModelIndex index;

	for (int i = 0; i < model_d->rowCount(); i++)
	{
		index = model_d->index(i, 0);
		QString current_name = model_d->data(index).toString();
		if (current_name.compare(ui.nameLabel->text()) == 0)
		{
			index = model_d->index(i, 1);
			break;
		}
	}

	photo.loadFromData(model_d->data(index).toByteArray(), "JPG");
	ui.photoLabel->setPixmap(photo);
}

void MedicalSystem::showDoctorBrief(void)
{
	QModelIndex index;

	for (int i = 0; i < model_d->rowCount(); i++)
	{
		index = model_d->index(i, 0);
		QString current_name = model_d->data(index).toString();
		if (current_name.compare(ui.nameLabel->text()) == 0)
		{
			index = model_d->index(i, 3);
			break;
		}
	}

	ui.briefTextEdit->setText(model_d->data(index).toString());
	ui.briefTextEdit->setFont(QFont("楷体", 12));
}

void MedicalSystem::showDoctorTime(void)
{
	QModelIndex index;

	for (int i = 0; i < model_d->rowCount(); i++)
	{
		index = model_d->index(i, 0);
		QString current_name = model_d->data(index).toString();
		if (current_name.compare(ui.nameLabel->text()) == 0)
		{
			index = model_d->index(i, 2);
			break;
		}
	}

	QString surgery = model_d->data(index).toString();
	for (uint8_t i = 0; i < surgery.length(); i++)
	{
		uint8_t num = surgery.mid(i, 1).toInt();
		if (num > 0)
		{
			if (num < 6)
			{
				ui.timeTableWidget->setItem(i % 2, i / 2,
					new QTableWidgetItem(QString::fromLocal8Bit("可预约")));
			}
			else
			{
				ui.timeTableWidget->setItem(i % 2, i / 2,
					new QTableWidgetItem(QString::fromLocal8Bit("已约满")));
			}			
		}
		else
		{
			ui.timeTableWidget->setItem(i%2, i/2, new QTableWidgetItem(""));
		}
	}		
}

bool MedicalSystem::checkPatientTime(uint8_t order)
{
	QModelIndex index;

	model_p->setTable("patient_inf");
	model_p->select();

	for (int i = 0; i < model_p->rowCount(); i++)
	{
		index = model_p->index(i, 1);
		QString current_name = model_p->data(index).toString();
		if (current_name.compare(ui.patientLineEdit->text()) == 0)
		{
			index = model_p->index(i, 4);
			break;
		}
	}

	QString time = model_p->data(index).toString();
	uint8_t num = time.mid(order, 1).toInt();

	if (num == 1)
	{
		return false;
	}
	else
	{
		time.replace(order, 1, QString::number(1));
		QString cmd = tr("update register.patient set time = '%1'"
			"where name = '%2';").arg(time).arg(ui.patientLineEdit->text());
		QSqlQuery query;	//定义数据库类
		query.exec(cmd);	//执行数据指令
		return true;
	}		
}

void MedicalSystem::on_timeUpdate(void)
{
	date = QDate::currentDate();
	time = QTime::currentTime();

	int year = date.year();
	int month = date.month();
	int day = date.day();

	ui.yearLcdNumber->display(year);
	ui.monthLcdNumber->display(month);
	ui.dayLcdNumber->display(day);
	ui.timeEdit->setTime(time);
}

void MedicalSystem::on_doctorTableView_clicked(const QModelIndex& index)
{
	QDate date;
	QModelIndex moedlIndex;

	int row = 0;
	if (index.data() != 0)
		row = ui.doctorTableView->currentIndex().row();

	moedlIndex = model->index(row, 0);		//姓名
	ui.nameLabel->setText(model->data(moedlIndex).toString());
	moedlIndex = model->index(row, 1);		//性别
	QString sex = model->data(moedlIndex).toString();
	(sex.compare(QString::fromLocal8Bit("男")) == 0) ?
		ui.maleRadioButton->setChecked(true) : ui.femaleRadioButton->setChecked(true);

	moedlIndex = model->index(row, 2);		//出生日期	
	int now = date.currentDate().year();
	int birth = model->data(moedlIndex).toDate().year();
	ui.ageSpinBox->setValue(now - birth);

	moedlIndex = model->index(row, 4);		//职称
	QString title = model->data(moedlIndex).toString();
	ui.titleComboBox->setCurrentText(title);

	showDoctorPhoto();		//照片
	showDoctorBrief();		//简介
	showDoctorTime();		//坐诊时间
}

void MedicalSystem::on_timeTableWidget_itemClicked(QTableWidgetItem* item)
{
	if (isLogin == false)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("请先登录，否则无法预约！"));
		return;
	}

	QString data = item->text();
	if (data.compare(QString::fromLocal8Bit("可预约")) == 0)
	{
		uint8_t row = item->row();
		uint8_t col = item->column();
		uint8_t site = col * 2 + row;

		if (checkPatientTime(site) == false)
		{
			QMessageBox::critical(0, QString::fromLocal8Bit("错误"),
				QString::fromLocal8Bit("您已预约过当前时段。"));
			return;
		}

		QString cmd = tr("select info from register.doctor "
			"where name = '").append(ui.nameLabel->text()).append("';");
		QSqlQuery query;	//定义数据库类
		query.exec(cmd);	//执行数据指令
		if (query.next())
		{
			QString info = query.value(0).toString();
			uint8_t num = info.mid(col * 2 + row, 1).toInt();
			num++;
			info.replace(col * 2 + row, 1, QString::number(num));
			if (num > 5)
			{
				ui.timeTableWidget->setItem(row, col,
					new QTableWidgetItem(QString::fromLocal8Bit("已约满")));
			}

			QString cmd = tr("update register.doctor set info = '%1'"
				"where name = '%2';").arg(info).arg(ui.nameLabel->text());
			QSqlQuery query;	//定义数据库类
			query.exec(cmd);	//执行数据指令
		}	

		QMessageBox::information(0, QString::fromLocal8Bit("成功"),
			QString::fromLocal8Bit("您已成功预约当前时段！"));
	}
	else if (data.compare("") == 0)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("当前时段无法预约，请选其他时段或医生。"));
	}
	else
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("当前时段已约满，请选其他时段或医生。"));
	}
}

void MedicalSystem::on_patientPushButton_clicked(void)
{
	QModelIndex index;

	for (int i = 0; i < model_p->rowCount(); i++)
	{
		index = model_p->index(i, 1);
		QString current_name = model_p->data(index).toString();
		if (current_name.compare(ui.patientLineEdit->text()) == 0)
		{
			isLogin = true;
			break;
		}		
	}

	if (isLogin == true)
	{
		QMessageBox::information(0, QString::fromLocal8Bit("成功"),
			QString::fromLocal8Bit("登录成功！"));
	}
	else
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("错误"),
			QString::fromLocal8Bit("无法查询您的信息！"));
	}
}

void MedicalSystem::on_treeWidget_itemClicked(QTreeWidgetItem* item, int column)
{
	QTreeWidgetItem* parent = item->parent();
	
	if (NULL == parent)
	{
		model->setTable("basic_inf");
		model->select();
		return;
	}
	
	QString department = item->text(column);
	model->setFilter(QString::fromLocal8Bit("部门 = '%1'").arg(department));
	model->select();	
}