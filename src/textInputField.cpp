#include "textInputField.h"
#include "client.h"

TextInputField::TextInputField(const std::string name, olc::vf2d position, const std::string decal_path, bool resize, int default_size, float size, int alternative_size, int left_text_margin, int top_text_margin, const std::string left_part_path, const std::string middle_part_path, const std::string right_part_path)
: Button(name, position, decal_path, "", false, size, default_size, left_text_margin, top_text_margin), resize(resize), default_size(default_size){
}

bool TextInputField::draw(){

	bool bShift = app->GetKey(olc::Key::SHIFT).bHeld;

    if(app->GetMouse(0).bPressed && !mouse_inside) focus = false;

    if(focus){
        for (int i = 1; i < 27; i++){
            if (app->GetKey((olc::Key)i).bPressed) {
                if(!resize && (int)text.length() != default_size - 1){
                    text += ((bShift ? (char)(i + 64) : (char)(i + 96)));
                }
                else if(resize){
                    text += ((bShift ? (char)(i + 64) : (char)(i + 96)));
                    if((int)text.length() >= default_size) alternative_size += size;
                }
            }
        }
        for (int i = 27; i < 37; i++){
            if (app->GetKey((olc::Key)i).bPressed) {
                if(!resize && (int)text.length() != default_size - 1){
                    text += ((char)(i + 48 - 27));
                }else if(resize){
                    text += ((char)(i + 48 - 27));
                    if((int)text.length() >= default_size) alternative_size += size;
                }
            }
        }
        if (app->GetKey(olc::PERIOD).bPressed) {
                if(!resize && (int)text.length() != default_size - 1){
                    text += ".";
                }else if(resize){
                    text += ".";
                    if((int)text.length() >= default_size) alternative_size += size;
                }    
        }

        if(app->GetKey(olc::BACK).bPressed){
            if((int)text.length() != 0){
                if(!resize){
                    text.pop_back();
                }else if(resize){
                    text.pop_back();
                    if(alternative_size != default_size * size){
                        alternative_size -=  size;
                    }
                }
            }
        }
        //if((int)text.length() == alternative_size && resize_button_to_text) alternative_size = (int)text.length() + 1;
                
        if(app->GetKey(olc::ENTER).bPressed && onEnter != NULL)onEnter();
        else enter_pressed = false;
    }

    Button::text = this->getText();

    Button::draw();

    return true;
}

void TextInputField::setOnEnter(std::function<void()> onEnter){
    this->onEnter = onEnter;
}

void TextInputField::onMouseClick(){
    Button::onMouseClick();
    focus = true;
}

std::string TextInputField::getText(){
    return text;
}

bool TextInputField::getEnterPressed(){
    return enter_pressed;
}
