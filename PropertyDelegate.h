#pragma once
#include <QtWidgets/QStyledItemDelegate>

enum PropertyType_t
{
	ui_default = 0,                   //Ä¬ÈÏÊý¾Ý
	ui_LineEditor = 1000,		      //ÆÕÍ¨µÄ±à¼­¿ò
	ui_LineEditor_Disabled = 1001,    //½ûÖ¹µÄ±à¼­¿ò
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
		switch (pTableWidget->item(index.row(), index.column())->type())
		{
		case ui_default:
			break;
		case ui_LineEditor_Disabled:
		{
			QLineEdit* editor = new QLineEdit(parent);
			editor->setEnabled(false);
			return editor;
		}
			break;
		default:
			break;
		}
	
		return QStyledItemDelegate::createEditor(parent, option, index);
	}

	void setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		switch (pTableWidget->item(index.row(), index.column())->type())
		{
		case ui_LineEditor_Disabled:
		{
			QString value = index.model()->data(index, Qt::EditRole).toString();
			QLineEdit* pLineEditor = static_cast<QLineEdit*>(editor);
			pLineEditor->setText(value);
			return;
		}
		default:
			break;
		}
	
		return QStyledItemDelegate::setEditorData(editor, index);
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const
	{
		//To do...
		switch (pTableWidget->item(index.row(), index.column())->type())
		{

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