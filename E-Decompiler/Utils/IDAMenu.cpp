#include "IDAMenu.h"
#include <loader.hpp>

IDAMenu::IDAMenu(const char* menuPath, int (*callback)(void*), void* ud,const char* shortcut, const char* tooltip, int icon)
{
	qstring str_menuPath = menuPath;
	qstring str_FirstMenu;

	int nIndex = str_menuPath.rfind('/');
	if (nIndex == qstring::npos) {
		m_actionLabel = str_menuPath;
		m_actionName = qstring(PLUGIN.wanted_name) + ":" + m_actionLabel;

		m_menuLabel = qstring(PLUGIN.wanted_name);
		m_menuName = qstring(PLUGIN.wanted_name) + ":" + m_menuLabel;

		str_FirstMenu = m_menuLabel;
	}
	else {
		m_actionLabel = str_menuPath.substr(nIndex + 1);
		m_actionName = qstring(PLUGIN.wanted_name) + ":" + m_actionLabel;

		m_menuLabel = str_menuPath.substr(0, nIndex);
		m_menuName = qstring(PLUGIN.wanted_name) + ":" + m_menuLabel;

		nIndex = str_menuPath.find('/');
		str_FirstMenu = str_menuPath.substr(0, nIndex);
	}

	create_menu(m_menuName.c_str(), str_FirstMenu.c_str());
	
	const action_desc_t desc = {
		sizeof(action_desc_t),
		m_actionName.c_str(),
		m_actionLabel.c_str(),
		this,
		&PLUGIN,
		shortcut,
		tooltip,
		icon,
		ADF_OT_PLUGIN
	};
	register_action(desc);

	qstring str_MenuPath = m_menuLabel;
	if (m_menuLabel.last() != '/') {
		str_MenuPath.append('/');
	}
	attach_action_to_menu(str_MenuPath.c_str(), m_actionName.c_str(), SETMENU_APP);

	m_callback = callback;
	m_userData = ud;
}

IDAMenu::~IDAMenu()
{
	unregister_action(m_actionName.c_str());
	delete_menu(m_menuName.c_str());
}

int IDAMenu::activate(action_activation_ctx_t* ctx)
{
	return m_callback(m_userData);
}

action_state_t IDAMenu::update(action_update_ctx_t* ctx)
{
	return AST_ENABLE_ALWAYS;
}