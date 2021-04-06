#pragma once
#include <QtWidgets/QStyledItemDelegate>

enum PropertyType_t
{
	ui_default = 0,                   //默认数据
	ui_LineEditor = 1000,		      //普通的编辑框
	ui_LineEditor_Disabled = 1001,    //禁止的编辑框
	ui_ColorDialog = 1002,            //颜色选择框
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
			case ui_ColorDialog:
			{
				QColorDialog* ret = new QColorDialog(parent);
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
					pItem->setText(QStringLiteral("默认底色"));
				}
				else {
					pItem->setText("");
				}
				
				pColorDiag->setCurrentColor(value);
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
		editor->setGeometry(option.rect);
	}
private:
	QTableWidget* pTableWidget = nullptr;
};