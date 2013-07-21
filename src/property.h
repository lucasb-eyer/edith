#ifndef _PROPERTY_H
#define _PROPERTY_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "bitstream.h"
#include "state.h"

class Property {
public:
  static std::shared_ptr<Property> read_prop(Bitstream &stream, const SendProp *prop, std::string& prop_name);

  Property(SP_Types type);
  virtual ~Property();

  template<typename T>
  decltype(T::value) value_as() const {
      return dynamic_cast<const T&>(*this).value;
  }

  SP_Types type;
};

std::ostream& operator<<(std::ostream& os, const Property& prop);

template<SP_Types SendPropType, typename T>
class TypedProperty : public Property {
public:
  TypedProperty(T _value) :
      Property(SendPropType), value(_value) {
  }

  T value;
};

template<SP_Types SendPropType, typename T, size_t C>
class FixedTypedProperty : public Property {
public:
  FixedTypedProperty(T _values[C]) :
      Property(SendPropType) {
    std::copy(_values, _values + C, values);
  }

  T values[C];

  std::ostream& print(std::ostream& os) const {
      os << "[";
      for(int i = 0 ; i < C ; ++i) {
          os << values[i] << (i < C-1 ? ", " : "");
      }
      return os << "]";
  }
};

typedef std::shared_ptr<Property> ArrayPropertyElement;

template<>
class TypedProperty<SP_Array, ArrayPropertyElement> : public Property {
public:
  TypedProperty(std::vector<ArrayPropertyElement> _elements, SP_Types _value_type) :
      Property(SP_Array),
      elements(_elements),
      value_type(_value_type) {
  }

  std::vector<ArrayPropertyElement> elements;
  SP_Types value_type;

  std::ostream& print(std::ostream& os) const {
      os << "[";
      for(auto iElem = elements.begin() ; iElem != elements.end() ; ++iElem) {
          os << **iElem << (iElem + 1 == elements.end() ? ", " : "");
      }
      return os << "]";
  }
};

typedef TypedProperty<SP_Int, uint32_t> IntProperty;
typedef TypedProperty<SP_Float, float> FloatProperty;
typedef FixedTypedProperty<SP_Vector, float, 3> VectorProperty;
typedef FixedTypedProperty<SP_VectorXY, float, 2> VectorXYProperty;
typedef TypedProperty<SP_String, std::string> StringProperty;
typedef TypedProperty<SP_Array, ArrayPropertyElement> ArrayProperty;
typedef TypedProperty<SP_Int64, uint64_t> Int64Property;

#endif

