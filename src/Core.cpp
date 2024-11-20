//----------------------------

#include "Core.hpp"

//----------------------------

Core::Core() {

}

//----------------------------

Core::~Core() {

}

//----------------------------

void Core::initGUI() {

    m_childWindow = tgui::ChildWindow::create("add Node");
    m_childWindow->setSize({ 200, 120 });

    tgui::Button::Ptr bt = tgui::Button::create("add");

    float shift = 20;

    for(int i = 0; i < 2; ++i) {
        m_edit[i] = tgui::EditBox::create();
        m_edit[i]->setPosition({ 20, shift });
        shift += m_edit[i]->getSize().y + 5;

        m_childWindow->add(m_edit[i]);
    }

    bt->setPosition({ 20 + m_edit[1]->getSize().x - bt->getSize().x, m_edit[1]->getPosition().y + m_edit[1]->getSize().y + 5 });

    m_edit[0]->setDefaultText("key");
    m_edit[0]->setInputValidator("[0-9]*");
    m_edit[1]->setDefaultText("data");
    m_edit[1]->setInputValidator("[a-zA-Z]");

    bt->onPress([&](){
        if(m_edit[0]->getText().empty() || m_edit[1]->getText().empty()) {
            m_gui.remove(m_childWindow);
            m_childWindow->setVisible(false);
            return;
        }

        int key;
        try {
            key = std::stoi(m_edit[0]->getText().toStdString());
        } catch(...) {  }

        if(!m_map.add(key, m_edit[1]->getText()[0])) {

            this->showMessageBox("info", "The key already exists", "OK");

            m_gui.remove(m_childWindow);
            m_childWindow->setVisible(false);

            return;
        }

        LOG(Core, "add node");

        m_render.setTargetTree(m_map.getRoot());
        m_gui.remove(m_childWindow);

        m_childWindow->setVisible(false);
    });

    m_childWindow->add(bt);

    m_childWindow->onClose([&](){ m_childWindow->setVisible(false); });

    m_popupMenu = tgui::ListBox::create();
    m_popupMenu->addItem("Add node");
    m_popupMenu->setItemHeight(20);
    m_popupMenu->setSize(120, m_popupMenu->getItemHeight() * m_popupMenu->getItemCount() + 2);

    m_popupMenu->onItemSelect([&](tgui::String select){

        m_popupMenu->deselectItem();
        m_popupMenu->setVisible(false);

        m_gui.add(m_childWindow);

        m_childWindow->setPosition(m_window.getSize().x / 2.0f - 100, m_window.getSize().y / 2.0f - 120 / 2);

        m_edit[0]->setText("");
        m_edit[1]->setText("");

        m_childWindow->setVisible(true);
    });

    m_popupMenu->setVisible(false);

    m_gui.add(m_popupMenu);

    m_findElement = tgui::ChildWindow::create("count element");
    m_findElement->setSize({ 200, 120 });

    tgui::Button::Ptr button = tgui::Button::create("find");
    button->setPosition({ 200 - (button->getSize().x + 5), 100 - (button->getSize().y + 5) });
    m_findElement->add(button);

    tgui::EditBox::Ptr edit = tgui::EditBox::create();
    edit->setDefaultText("data element");
    edit->setPosition({ 200 / 2.f - edit->getSize().x / 2.0f, 100 / 2.0f - edit->getSize().y / 2.0f });
    edit->setInputValidator("[a-zA-Z]");

    m_findElement->add(edit);
    m_findElement->setVisible(false);

    button->onPress([=]{
        this->showMessageBox("find (" + edit->getText() + ")", std::to_string(m_map.getCountElement(edit->getText()[0])), "OK");
        m_gui.remove(m_findElement);
        m_findElement->setVisible(false);
    });

    m_findElement->onClose([&](){ m_findElement->setVisible(false); });
}

//----------------------------

void Core::showMessageBox(tgui::String title, tgui::String text, tgui::String button) {

    m_msg = tgui::MessageBox::create(title);
    m_msg->setText(text);
    m_msg->addButton(button);
    m_msg->setPosition(m_window.getSize().x / 2.0f - m_msg->getSize().x/2.0f, m_window.getSize().y / 2.0f - m_msg->getSize().y/2.0f);

    if(m_msg->getSize().x < 120)
        m_msg->setSize({ 120, m_msg->getSize().y});

    m_msg->onButtonPress([&](const tgui::String& button){
        m_msg->getParent()->remove(m_msg->shared_from_this());
        m_msg = nullptr;
    });

    m_gui.add(m_msg);
}

//----------------------------

bool Core::init(sf::Vector2u resolution) {

    if(!m_font.loadFromFile("./Arialuni.ttf"))
        return false;

    std::string title = TITLE;
    title += " ";
    title += VERSION;

    m_window.create(sf::VideoMode(resolution.x, resolution.y), title, sf::Style::Close);
    m_window.setVerticalSyncEnabled(true);
    m_window.setFramerateLimit(60);

    m_map.add(1, 'a');
    m_map.add(2, 'a');
    m_map.add(3, 'a');
    m_map.add(4, 'a');
    m_map.add(5, 'a');

    m_map.remove(1);

    for(uint8_t i = 6; i < 30; ++i)
        m_map.add(i, 'a' + i);

    m_map.debugPrint();

    m_render.init(m_font, m_window);
    m_render.setTargetTree(m_map.getRoot());

    m_gui.setTarget(m_window);

    this->initGUI();

    return true;
}

//----------------------------

void Core::draw() {
    m_window.clear({ 80, 80, 80 });

    m_window.draw(m_render);
    m_gui.draw();

    m_window.display();
}

//----------------------------

void Core::update() {

    sf::Event event;

    while(m_window.pollEvent(event)) {

        if(event.type == sf::Event::Closed)
            m_window.close();

        else if(event.type == sf::Event::KeyPressed) {
            if(event.key.code == sf::Keyboard::Escape)
                m_window.close();

            else if(event.key.code == sf::Keyboard::Delete) {

                TreeRenderer::Item* focus = m_render.getFocus();
                if(focus) {
                    m_map.remove(focus->key);
                    m_render.setTargetTree(m_map.getRoot());
                }
            }

            else if(event.key.code == sf::Keyboard::F1) {

                tgui::String str;
                str += "inorder: ";

                str += m_map.inorder();
                str += "\npreorder: ";
                str += m_map.preorder();
                str += "\npostorder: ";
                str += m_map.postorder();

                this->showMessageBox("inorder, preorder, postorder", str, "OK");
            }

            else if(event.key.code == sf::Keyboard::F2) {
                if(m_msg == nullptr)
                    this->showMessageBox("print horizontal", m_map.getPrintHorizontal(), "OK");
            }

            else if(event.key.code == sf::Keyboard::F3) {
                if(m_msg == nullptr)
                    this->showMessageBox("print vertical", m_map.getPrintVertical(), "OK");
            }

            else if(event.key.code == sf::Keyboard::F4) {

                if(m_msg != nullptr)
                    return;

                m_findElement->setPosition( m_window.getSize().x / 2.0f - m_findElement->getSize().x/2.0f,
                                            m_window.getSize().y / 2.0f - m_findElement->getSize().y/2.0f);

                m_gui.add(m_findElement);
                m_findElement->setVisible(true);
            }
        }

        else if(event.type == sf::Event::MouseButtonPressed) {
            if(event.mouseButton.button == sf::Mouse::Right) {
                m_popupMenu->setVisible(true);

                sf::Vector2f pos = { (float)event.mouseButton.x, (float)event.mouseButton.y };
                m_popupMenu->setPosition({ pos.x, pos.y });

            }

            else if(event.mouseButton.button == sf::Mouse::Left) {
                if(m_childWindow->isVisible() && m_childWindow->isMouseOnWidget(tgui::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                    m_press = true;
                } else {
                    m_press = false;
                }

                if(m_msg && m_msg->isMouseOnWidget(tgui::Vector2f(event.mouseButton.x, event.mouseButton.y)) )
                    m_press = true;

                if(m_findElement->isVisible() && m_findElement->isMouseOnWidget(tgui::Vector2f(event.mouseButton.x, event.mouseButton.y)) )
                    m_press = true;

                if(!m_popupMenu->isMouseOnWidget(tgui::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                    m_popupMenu->setVisible(false);
                }
            }
        }

        m_gui.handleEvent(event);

        if(!m_press)
            m_render.handleEvent(event);
    }

    m_render.update(m_clock.restart());
}

//----------------------------
