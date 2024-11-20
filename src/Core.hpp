//----------------------------

#ifndef CORE_HPP_INCLUDED
#define CORE_HPP_INCLUDED

//----------------------------

#include "included_libraries.hpp"

#include "TreeRenderer.hpp"
#include "Map.hpp"

//----------------------------

class Core {
public:

    Core();
    ~Core();

    bool init(sf::Vector2u resolution);

    void draw();
    void update();

    bool windowIsOpen() { return m_window.isOpen(); }

private:

    sf::RenderWindow m_window;
    sf::Font m_font;

    Map<int, char> m_map;
    TreeRenderer m_render;

    sf::Clock m_clock;

    tgui::Gui m_gui;

    tgui::ListBox::Ptr m_popupMenu;
    tgui::ChildWindow::Ptr m_childWindow;
    tgui::ChildWindow::Ptr m_findElement;

    tgui::EditBox::Ptr m_edit[2];

    tgui::MessageBox::Ptr m_msg;

    bool m_press = false;

    void initGUI();
    void showMessageBox(tgui::String title, tgui::String text, tgui::String button);
};

//----------------------------

#endif // CORE_HPP_INCLUDED

//----------------------------
