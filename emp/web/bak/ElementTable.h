#ifndef EMP_UI_ELEMENT_TABLE_H
#define EMP_UI_ELEMENT_TABLE_H

///////////////////////////////////////////////////////////////////////////////////////////
//
//  Manage a section of the current web page
//

#include <functional>
#include <string>
#include <vector>

#include <emscripten.h>

#include "../tools/DynamicStringSet.h"

#include "Element.h"

namespace emp {
namespace UI {

  class ElementTable : public Element, public Table {
  public:

    virtual bool OK(std::stringstream & ss, bool verbose=false, const std::string & prefix="") override {
      bool ok = true;

      if (verbose) {
        ss << prefix << "Scanning: emp::UI::ElementTable with name = '" << name << "'" << std::endl;
      }

      Element::OK(ss, verbose, prefix);  // Check base Element class; same obj, don't change prefix
      Table::OK(ss, verbose, prefix);    // Check base Table class.

      return ok;
    }

  };

};
};

#endif
