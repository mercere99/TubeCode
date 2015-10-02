#ifndef EMP_WEB_ELEMENT_H
#define EMP_WEB_ELEMENT_H

/////////////////////////////////////////////////////////////////////////////////////////
//
//  Base class for a single element on a web page (a paragraph, a button, a table, etc.)
//

#include <emscripten.h>
#include <sstream>
#include <string>
#include <typeinfo>

#include "../tools/alert.h"
#include "../tools/assert.h"
#include "../tools/string_utils.h"
#include "../tools/vector.h"

#include "events.h"
#include "UI_base.h"

namespace emp {
namespace UI {

  class Element {
  protected:


  public:
    Element * GetParent() { return parent; }

    // Functions to access children
    int GetNumChildren() const { return children.size(); }
    Element & GetChild(int id) {
      emp_assert(id >= 0 && id < children.size());
      return *(children[id]);
    }
    const Element & GetChild(int id) const {
      emp_assert(id >= 0 && id < children.size());
      return *(children[id]);
    }

    Element & SetParent(Element * p) { parent = p; return *this; }

    Element & PreventAppend() { append_ok = false; return *this; }


    // UpdateNow() refreshes the document immediately (and should only be called if that's okay!)
    // By default: call UpdateHTML (which should be overridden) print HTML_string, and UpdateCSS
    virtual void UpdateNow() {
      UpdateHTML();
      EM_ASM_ARGS({
          var elem_name = Pointer_stringify($0);
          var html_str = Pointer_stringify($1);
          $( '#' + elem_name ).html(html_str);
        }, GetName().c_str(), HTML.str().c_str() );
      UpdateCSS();
      UpdateJS();

      // Now that the parent is up-to-day, update all children.
      for (auto * child : children) child->UpdateNow();
    }


    // Update() refreshes the document once it's ready.
    void Update() {
      // OnDocumentReady( [this](){ this->UpdateNow(); } );
      OnDocumentReady( std::function<void(void)>([this](){ this->UpdateNow(); }) );
    }



    // Print out the contents of this element as HTML.
    virtual void PrintHTML(std::ostream & os) {
      UpdateHTML();
      os << HTML.str();
    }

    void AlertHTML() {
      std::stringstream ss;
      PrintHTML(ss);
      emp::Alert(ss.str());
    }

    
    virtual std::string GetType() = 0;

    // Make sure everything about this element is consistent and logical.
    // ss will include any warnings (conserns, but potentially ok) or errors (serious problems)
    // found.  If verbose is set to true, will also include many additional comments.

    virtual bool OK(std::stringstream & ss, bool verbose=false, const std::string & prefix="") {
      bool ok = true;
      if (verbose) {
        ss << prefix << "Scanning: emp::UI::Element with name = '" << name << "'" << std::endl;
      }

      if (parent != nullptr && parent->HasChild(this) == false) {
        ss << prefix << "ERROR: this element not listed as child of it's parent!" << std::endl;
        ok = false;
      }

      // Make sure all children are ok (they will report their own errors!
      for (int i = 0; i < (int) children.size(); i++) {
        auto & child = children[i];
        if (child == nullptr) {
          ss << prefix << "ERROR: Child element " << i << " has value 'nullptr'" << std::endl;
          ok = false;
        }
        else if (!child->OK(ss,verbose,prefix+"  ")) ok = false;
      }

      return ok;
    }
  };

};
};

#endif
