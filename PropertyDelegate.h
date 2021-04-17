#pragma once
#include <QtWidgets/QStyledItemDelegate>
#include <QVariant>

enum PropertyType_t
{
	ui_default = 0,                    //Ä¬ÈÏÊý¾Ý
	ui_LineEditor = 1000,              //ÆÕÍ¨µÄ±à¼­¿ò
	ui_LineEditor_Disabled = 1001,     //½ûÖ¹µÄ±à¼­¿ò
	ui_LineEditor_ReadOnly = 1002,     //Ö»¶ÁµÄ±à¼­¿ò
	ui_ColorDialog = 1003,             //ÑÕÉ«Ñ¡Ôñ¿ò
	ui_ComboBoxList = 1004,            //ÏÂÀ­¿ò
	ui_ImageBox = 1005,                //Í¼Æ¬¿ò
};

class PropertyDelegate :public QStyledItemDelegate
{
public:
	PropertyDelegate(QTableWidget* table) :QStyledItemDelegate()
	{
		pTableWidget = table;
	}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		QTableWidgetItem* pItem = pTableWidget->item(index.row(), index.column());
		
		if (pItem) {
			switch (pItem->type())
			{
			case ui_default:
				break;
			case ui_LineEditor_Disabled:
			{
				QLineEdit* ret = new QLineEdit(parent);
				ret->setEnabled(false);
				return ret;
			}
			case ui_LineEditor_ReadOnly:
			{
				QLineEdit* ret = new QLineEdit(parent);
				ret->setReadOnly(true);
				return ret;
			}
			case ui_ColorDialog:
			{
				QColorDialog* ret = new QColorDialog(parent);
				return ret;
			}
			case ui_ComboBoxList:
			{
				QComboBox* ret = new QComboBox(parent);
				pItem->setData(Qt::UserRole, 5);
				return ret;
			}
			case ui_ImageBox:
			{
				QLabel* ret = new QLabel(parent, Qt::Dialog | Qt::WindowCloseButtonHint);
				ret->setWindowTitle(QStringLiteral("Í¼Æ¬"));
				return ret;
			}
			default:
				break;
			}
		}
		return QStyledItemDelegate::createEditor(parent, option, index);
	}

	void setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		QTableWidgetItem* pItem = pTableWidget->item(index.row(), index.column());
		if (pItem) {
			switch (pItem->type())
			{
			case ui_LineEditor_Disabled:
			case ui_LineEditor_ReadOnly:
			{
				QString value = index.model()->data(index, Qt::EditRole).toString();
				QLineEdit* pLineEditor = static_cast<QLineEdit*>(editor);
				pLineEditor->setText(value);
				return;
			}
			case ui_ColorDialog:
			{
				QColorDialog* pColorDiag = static_cast<QColorDialog*>(editor);
				uint32 color = GetSysColor(COLOR_BTNFACE);
				QColor value = index.model()->data(index, Qt::BackgroundRole).value<QColor>();
				if (QColor(GetRValue(color), GetGValue(color), GetBValue(color)) == value) {
					pItem->setText(QStringLiteral("Ä¬ÈÏµ×É«"));
				}
				else {
					pItem->setText("");
				}
				
				pColorDiag->setCurrentColor(value);
				return;
			}
			case ui_ComboBoxList:
			{
				QComboBox* pComboBox = static_cast<QComboBox*>(editor);
				return;
			}
			case ui_ImageBox:
			{
				QImage imageValue = index.model()->data(index, Qt::UserRole).value<QImage>();
				QLabel* pLabel = static_cast<QLabel*>(editor);
				pLabel->setPixmap(QPixmap::fromImage(imageValue));
				pLabel->adjustSize();
				return;
			}
			default:
				break;
			}
		}
		return QStyledItemDelegate::setEditorData(editor, index);
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const
	{
		QTableWidgetItem* pItem = pTableWidget->item(index.row(), index.column());
		if (pItem) {
			switch (pItem->type())
			{
			case ui_ColorDialog:
			{
				QColorDialog* pColorDiag = static_cast<QColorDialog*>(editor);
				QColor color = pColorDiag->currentColor();
				model->setData(index, color, Qt::BackgroundRole);
				return;
			}
			default:
				break;
			}
		}
	

		return QStyledItemDelegate::setModelData(editor, model, index);
	}

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QTableWidgetItem* pItem = pTableWidget->item(index.row(), index.column());
		if (pItem) {
			switch (pItem->type())
			{
			case ui_ImageBox:
			{
				//Ê¹Í¼Æ¬Î»ÖÃ¾ÓÖÐ
				QRect centerRect;
				centerRect.setLeft((QApplication::desktop()->width() - option.rect.width()) / 2);
				centerRect.setTop((QApplication::desktop()->height() - option.rect.height()) / 2);
				editor->setGeometry(centerRect);
				return;
			}
			default:
				break;
			}
		}
		editor->setGeometry(option.rect);
	}
private:
	QTableWidget* pTableWidget = nullptr;
};