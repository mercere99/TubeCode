#ifndef EMP_UI_ELEMENT_SLATE_H
#define EMP_UI_ELEMENT_SLATE_H

#include "Animate.h"

namespace emp {
namespace UI {

  class ElementSlate : public Element, public Slate {
  protected:

    void UpdateHTML() override {
      HTML.str("");                               // Clear the current stream.
      for (auto * element : children) {
        const std::string & tag = element->GetWrapperTag();
        HTML << "<" << tag << " id=\"" << element->GetName() << "\"></" << tag << ">\n";
      }
    }
    void UpdateCSS() override {
      TriggerCSS();
    }
    
    // BuildElement allows any element to build another as long as an ancestor knows how.
    Element * BuildElement(emp::UI::Button info, Element * fwd_parent) override {
      return new ElementButton(info, fwd_parent);
    }
    Element * BuildElement(emp::UI::Canvas info, Element * fwd_parent) override {
      return new ElementCanvas(info, fwd_parent);
    }
    Element * BuildElement(emp::UI::Image info, Element * fwd_parent) override {
      return new ElementImage(info, fwd_parent);
    }
    Element * BuildElement(emp::UI::Selector info, Element * fwd_parent) override {
      return new ElementSelector(info, fwd_parent);
    }
    Element * BuildElement(emp::UI::Table info, Element * fwd_parent) override {
      return new ElementTable(info, fwd_parent);
    }
    Element * BuildElement(emp::UI::Text info, Element * fwd_parent) override {
      return new ElementText(info, fwd_parent);
    }
    Element * BuildElement(emp::UI::Slate info, Element * fwd_parent) override {
      return new ElementSlate(info, fwd_parent);
    }
      

public:
    ElementSlate(const Slate & in_slate, Element * in_parent=nullptr)
      : Element(in_slate.GetDivID(), in_parent), emp::UI::Slate(in_slate) { ; }
    ElementSlate(const std::string & in_name, Element * in_parent=nullptr)
      : Element(in_name, in_parent), emp::UI::Slate(in_name) { ; }
    ~ElementSlate() { ; }
    
    virtual Element * Clone(Element * parent, const std::string & ext) const override {
      return new ElementSlate(*this, parent, ext);
    };

    Element & operator[](const std::string & test_name) {
      emp_assert(Contains(test_name));
      return *(element_dict[test_name]);
    }



    // Add a an element to this slate that was created elsewhere.
    Element & AddChild(Element * child) {
      emp_assert(child != nullptr);
      child->SetParent(this);
      children.push_back(child);
      return *child;
    }


    // Build a new element of the specified type in this slate.
    // Add additional children on to this element.

    ElementButton & Add(UI::Button info) {
      ElementButton * new_child = new ElementButton(info, this);
      children.push_back(new_child);
      return *new_child;
    }
    ElementCanvas & Add(UI::Canvas info) {
      ElementCanvas * new_child = new ElementCanvas(info, this);
      children.push_back(new_child);
      return *new_child;
    }
    ElementImage & Add(UI::Image info) {
      ElementImage * new_child = new ElementImage(info, this);
      children.push_back(new_child);
      return *new_child;
    }
    ElementSelector & Add(UI::Selector info) {
      ElementSelector * new_child = new ElementSelector(info, this);
      children.push_back(new_child);
      return *new_child;
    }
    ElementSlate & Add(UI::Slate info) {
      ElementSlate * new_child = new ElementSlate(info, this);
      children.push_back(new_child);
      return *new_child;
    }
    ElementTable & Add(UI::Table info) {
      ElementTable * new_child = new ElementTable(info, this);
      children.push_back(new_child);
      return *new_child;
    }
    ElementText & Add(UI::Text info) {
      ElementText * new_child = new ElementText(info, this);
      children.push_back(new_child);
      return *new_child;
    }

    // Shortcut adders where parameters don't have to be widgets
    template <class... T> ElementButton& AddButton(T... args){return Add(UI::Button(args...));}
    template <class... T> ElementCanvas& AddCanvas(T... args){return Add(UI::Canvas(args...));}
    template <class... T> ElementImage&  AddImage(T... args) {return Add(UI::Image(args...));}
    template <class... T> ElementSelector&AddSelector(T... args){return Add(UI::Selector(args...));}
    template <class... T> ElementSlate&  AddSlate(T... args) {return Add(UI::Slate(args...));}
    template <class... T> ElementTable&  AddTable(T... args) {return Add(UI::Table(args...));}
    template <class... T> ElementText&   AddText(T... args)  {return Add(UI::Text(args...));}

    // Other helpful adders...
    ElementButton & AddAnimToggle(UI::Animate & anim,
                                  const std::string & play_label="Play",
                                  const std::string & pause_label="Pause",
                                  const std::string & button_id
                                  =emp::to_string("emp__", internal::NextWidgetID())) {
      UI::Button toggle_but = UI::Button([&anim, play_label, pause_label, button_id, this](){
          anim.ToggleActive();                           // Toggle state of animation.
          auto & but = Button(button_id);                // Lookup this button...
          if (anim.GetActive()) but.Label(pause_label);  // Setup proper label on button.
          else but.Label(play_label);
          but.Update();                                  // Redraw the button.
        }, play_label, button_id).Size(50,30);  
      return Add(toggle_but);
    }



    virtual std::string GetType() override {
      return "ElementSlate";
    }

    virtual bool OK(std::stringstream & ss, bool verbose=false, const std::string & prefix="") override {
      bool ok = true;

      if (verbose) {
        ss << prefix << "Scanning: emp::UI::ElementSlate with name = '" << name << "'" << std::endl;
      }

      Element::OK(ss, verbose, prefix);  // Check base class; same obj, don't change prefix

      // @CAO Make sure the element_dict is okay?

      return ok;
    }

  };

  using Document = emp::UI::ElementSlate;
};
};

#endif
