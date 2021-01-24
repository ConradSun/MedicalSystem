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
		
	timer->setInterval(1000);				//���ö�ʱ���������
	timer->start();							//���ö�ʱ����ʼֵ
		
	model->setTable("basic_inf");			//������Ϣ��ͼ
	model->select();
	model_d->setTable("detail_inf");		//������ϸ��Ϣ��ͼ
	model_d->select();
	model_p->setTable("patient_inf");
	model_p->select();
	
	connect(timer, SIGNAL(timeout()), this, SLOT(on_timeUpdate()));
	ui.doctorTableView->setModel(model);	//����������Ϣ����

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
	ui.briefTextEdit->setFont(QFont("����", 12));
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
					new QTableWidgetItem(QString::fromLocal8Bit("��ԤԼ")));
			}
			else
			{
				ui.timeTableWidget->setItem(i % 2, i / 2,
					new QTableWidgetItem(QString::fromLocal8Bit("��Լ��")));
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
		QSqlQuery query;	//�������ݿ���
		query.exec(cmd);	//ִ������ָ��
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

	moedlIndex = model->index(row, 0);		//����
	ui.nameLabel->setText(model->data(moedlIndex).toString());
	moedlIndex = model->index(row, 1);		//�Ա�
	QString sex = model->data(moedlIndex).toString();
	(sex.compare(QString::fromLocal8Bit("��")) == 0) ?
		ui.maleRadioButton->setChecked(true) : ui.femaleRadioButton->setChecked(true);

	moedlIndex = model->index(row, 2);		//��������	
	int now = date.currentDate().year();
	int birth = model->data(moedlIndex).toDate().year();
	ui.ageSpinBox->setValue(now - birth);

	moedlIndex = model->index(row, 4);		//ְ��
	QString title = model->data(moedlIndex).toString();
	ui.titleComboBox->setCurrentText(title);

	showDoctorPhoto();		//��Ƭ
	showDoctorBrief();		//���
	showDoctorTime();		//����ʱ��
}

void MedicalSystem::on_timeTableWidget_itemClicked(QTableWidgetItem* item)
{
	if (isLogin == false)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("���ȵ�¼�������޷�ԤԼ��"));
		return;
	}

	QString data = item->text();
	if (data.compare(QString::fromLocal8Bit("��ԤԼ")) == 0)
	{
		uint8_t row = item->row();
		uint8_t col = item->column();
		uint8_t site = col * 2 + row;

		if (checkPatientTime(site) == false)
		{
			QMessageBox::critical(0, QString::fromLocal8Bit("����"),
				QString::fromLocal8Bit("����ԤԼ����ǰʱ�Ρ�"));
			return;
		}

		QString cmd = tr("select info from register.doctor "
			"where name = '").append(ui.nameLabel->text()).append("';");
		QSqlQuery query;	//�������ݿ���
		query.exec(cmd);	//ִ������ָ��
		if (query.next())
		{
			QString info = query.value(0).toString();
			uint8_t num = info.mid(col * 2 + row, 1).toInt();
			num++;
			info.replace(col * 2 + row, 1, QString::number(num));
			if (num > 5)
			{
				ui.timeTableWidget->setItem(row, col,
					new QTableWidgetItem(QString::fromLocal8Bit("��Լ��")));
			}

			QString cmd = tr("update register.doctor set info = '%1'"
				"where name = '%2';").arg(info).arg(ui.nameLabel->text());
			QSqlQuery query;	//�������ݿ���
			query.exec(cmd);	//ִ������ָ��
		}	

		QMessageBox::information(0, QString::fromLocal8Bit("�ɹ�"),
			QString::fromLocal8Bit("���ѳɹ�ԤԼ��ǰʱ�Σ�"));
	}
	else if (data.compare("") == 0)
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("��ǰʱ���޷�ԤԼ����ѡ����ʱ�λ�ҽ����"));
	}
	else
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("��ǰʱ����Լ������ѡ����ʱ�λ�ҽ����"));
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
		QMessageBox::information(0, QString::fromLocal8Bit("�ɹ�"),
			QString::fromLocal8Bit("��¼�ɹ���"));
	}
	else
	{
		QMessageBox::critical(0, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("�޷���ѯ������Ϣ��"));
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
	model->setFilter(QString::fromLocal8Bit("���� = '%1'").arg(department));
	model->select();	
}