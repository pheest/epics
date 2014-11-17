/* pvIntrospect.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk and Michael Davidsaver
 */
#ifndef PVINTROSPECT_H
#define PVINTROSPECT_H
#include <string>
#include <stdexcept>

#include <pv/noDefaultMethods.h>
#include <pv/pvType.h>
#include <pv/byteBuffer.h>
#include <pv/serialize.h>
#include <sharelib.h>

namespace epics { namespace pvData { 

class Field;
class Scalar;
class ScalarArray;
class Structure;
class StructureArray;

/**
 * typedef for a shared pointer to an immutable Field.
 */
typedef std::tr1::shared_ptr<const Field> FieldConstPtr;
/**
 * typedef for an array of shared pointer to an immutable Field.
 */
typedef std::vector<FieldConstPtr> FieldConstPtrArray;
/**
 * typedef for a shared pointer to an immutable Scalar.
 */
typedef std::tr1::shared_ptr<const Scalar> ScalarConstPtr;
/**
 * typedef for a shared pointer to an immutable ScalarArray.
 */
typedef std::tr1::shared_ptr<const ScalarArray> ScalarArrayConstPtr;
/**
 * typedef for a shared pointer to an immutable Structure.
 */
typedef std::tr1::shared_ptr<const Structure> StructureConstPtr;
/**
 * typedef for a shared pointer to an immutable StructureArray.
 */
typedef std::tr1::shared_ptr<const StructureArray> StructureArrayConstPtr;

/**
 * Definition of support field types.
 */
enum Type {
    /**
     * The type is scalar. It has a scalarType
     */
    scalar,
    /**
     * The type is scalarArray. Each element is a scalar of the same scalarType.
     */
    scalarArray,
    /**
     * The type is structure.
     */
    structure,
    /**
     * The type is structureArray. Each element is a structure.
     */
    structureArray
};

/**
 * Convenience functions for Type.
 */
namespace TypeFunc {
    /**
     * Get a name for the type.
     * @param  type The type.
     * @return The name for the type.
     */
    epicsShareExtern const char* name(Type type);
    /**
     * Convert the type to a string and add it to builder.
     * @param  builder The string builder.
     * @param  type    The type.
     */
    epicsShareExtern void toString(StringBuilder builder,const Type type);
};

/**
 * Definition of support scalar types.
 */
enum ScalarType {
    /**
     * The type is boolean, i. e. value can be {@code false} or {@code true}
     */
    pvBoolean,
    /**
     * The type is byte, i. e. a 8 bit signed integer.
     */
    pvByte,
    /**
     * The type is short, i. e. a 16 bit signed integer.
     */
    pvShort,
    /**
     * The type is int, i. e. a 32 bit signed integer.
     */
    pvInt,
    /**
     * The type is long, i. e. a 64 bit signed integer.
     */
    pvLong,
    /**
     * The type is unsigned byte, i. e. a 8 bit unsigned integer.
     */
    pvUByte,
    /**
     * The type is unsigned short, i. e. a 16 bit unsigned integer.
     */
    pvUShort,
    /**
     * The type is unsigned int, i. e. a 32 bit unsigned integer.
     */
    pvUInt,
    /**
     * The type is unsigned long, i. e. a 64 bit unsigned integer.
     */
    pvULong,
    /**
     * The type is float, i. e. 32 bit IEEE floating point,
     */
    pvFloat,
    /**
     * The type is float, i. e. 64 bit IEEE floating point,
     */
    pvDouble,
    /**
     * The type is string, i. e. a UTF8 character string.
     */
    pvString
};

/**
 * Convenience functions for ScalarType.
 */
namespace ScalarTypeFunc {
    /**
     * Is the type an integer, i. e. is it one of byte,...ulong
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is an integer.
     */
    epicsShareExtern bool isInteger(ScalarType scalarType);
    /**
     * Is the type an unsigned integer, i. e. is it one of ubyte,...ulong
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is an integer.
     */
    epicsShareExtern bool isUInteger(ScalarType scalarType);
    /**
     * Is the type numeric, i. e. is it one of byte,...,double
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is a numeric
     */
    epicsShareExtern bool isNumeric(ScalarType scalarType);
    /**
     * Is the type primitive, i. e. not string
     * @param  scalarType The type.
     * @return (false,true) if the scalarType is primitive.
     */
    epicsShareExtern bool isPrimitive(ScalarType scalarType);
    /**
     * Get the scalarType for value.
     * @param  value The name of the scalar type.
     * @return The scalarType.
     * An exception is thrown if the name is not the name of a scalar type.
     */
    epicsShareExtern ScalarType getScalarType(String const &value);
    /**
     * Get a name for the scalarType.
     * @param  scalarType The type.
     * @return The name for the scalarType.
     */
    epicsShareExtern const char* name(ScalarType scalarType);
    /**
     * Convert the scalarType to a string and add it to builder.
     * @param  builder The string builder.
     * @param  scalarType    The type.
     */
    epicsShareExtern void toString(StringBuilder builder,ScalarType scalarType);
};

/**
 * This class implements introspection object for field.
 */
class epicsShareClass Field : 
    virtual public Serializable,
    public std::tr1::enable_shared_from_this<Field> {
public:
   POINTER_DEFINITIONS(Field);
    /**
     * Destructor.
     */
   virtual ~Field();
    /**
     * Get the field type.
     * @return The type.
     */
   Type getType() const{return m_fieldType;}
   /**
    * Get the identification string.
    * @return The identification string, can be empty.
    */
   virtual String getID() const = 0;
    /**
     * Convert the scalarType to a string and add it to builder.
     * @param  builder The string builder.
     */
   virtual void toString(StringBuilder builder) const{toString(builder,0);}
    /**
     * Convert the scalarType to a string and add it to builder.
     * @param  builder The string builder.
     * @param  indentLevel The number of blanks at the beginning of new lines.
     */
   virtual void toString(StringBuilder builder,int indentLevel) const;
protected:
    /**
     * Constructor
     * @param  fieldName The field type.
     */
   Field(Type type);
private:
   Type m_fieldType;

   friend class StructureArray;
   friend class Structure;
   friend class PVFieldPvt;
   friend class StandardField;
   friend class BasePVStructureArray;
   friend class FieldCreate;

   struct Deleter{void operator()(Field *p){delete p;}};
};


/**
 * This class implements introspection object for Scalar.
 */
class epicsShareClass Scalar : public Field{
public:
    POINTER_DEFINITIONS(Scalar);
    /**
     * Destructor.
     */
    virtual ~Scalar();
    typedef Scalar& reference;
    typedef const Scalar& const_reference;
    /**
     * Get the scalarType
     * @return the scalarType
     */
    ScalarType getScalarType() const {return scalarType;}
    /**
     * Convert the scalar to a string and add it to builder.
     * @param  builder The string builder.
     */
    virtual void toString(StringBuilder buf) const{toString(buf,0);}
    /**
     * Convert the scalar to a string and add it to builder.
     * @param  builder The string builder.
     * @param  indentLevel The number of blanks at the beginning of new lines.
     */
    virtual void toString(StringBuilder buf,int indentLevel) const;

    virtual String getID() const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);
    
protected:
    Scalar(ScalarType scalarType);
private:
    int8 getTypeCodeLUT() const;
    ScalarType scalarType;
    friend class FieldCreate;
};

/**
 * This class implements introspection object for field.
 */
class epicsShareClass ScalarArray : public Field{
public:
    POINTER_DEFINITIONS(ScalarArray);
    typedef ScalarArray& reference;
    typedef const ScalarArray& const_reference;

    /**
     * Constructor
     * @param scalarType The scalarType for the field.
     */
    ScalarArray(ScalarType scalarType);
    /**
     * Get the scalarType for the elements.
     * @return the scalarType
     */
    ScalarType  getElementType() const {return elementType;}
    /**
     * Convert the scalarType to a string and add it to builder.
     * @param  builder The string builder.
     */
    virtual void toString(StringBuilder buf) const{toString(buf,0);}
    /**
     * Convert the scalarType to a string and add it to builder.
     * @param  builder The string builder.
     * @param  indentLevel The number of blanks at the beginning of new lines.
     */
    virtual void toString(StringBuilder buf,int indentLevel) const;
    
    virtual String getID() const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);
    
protected:
    /**
     * Destructor.
     */
    virtual ~ScalarArray();
private:
    int8 getTypeCodeLUT() const;
    const String getIDScalarArrayLUT() const;
    ScalarType elementType;
    friend class FieldCreate;
};

/**
 * This class implements introspection object for a structureArray
 */
class epicsShareClass StructureArray : public Field{
public:
    POINTER_DEFINITIONS(StructureArray);
    typedef StructureArray& reference;
    typedef const StructureArray& const_reference;

    /**
     * Get the introspection interface for the array elements.
     * @return The introspection interface.
     */
    StructureConstPtr  getStructure() const {return pstructure;}

    /**
     * Convert the scalarType to a string and add it to builder.
     * @param  builder The string builder.
     * @param  indentLevel The number of blanks at the beginning of new lines.
     */
    virtual void toString(StringBuilder buf,int indentLevel=0) const;
    
    virtual String getID() const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);

protected:
    /**
     * Constructor.
     * @param structure The introspection interface for the elements.
     */
    StructureArray(StructureConstPtr const & structure);
    /**
     * Destructor.
     */
    virtual ~StructureArray();
private:
    StructureConstPtr pstructure;
    friend class FieldCreate;
};

/**
 * This class implements introspection object for a structure.
 */
class epicsShareClass Structure : public Field {
public:
    POINTER_DEFINITIONS(Structure);

    /**
     * Default structure ID.
     */
    static epics::pvData::String DEFAULT_ID;

    /**
     * Destructor.
     */
    virtual ~Structure();
    typedef Structure& reference;
    typedef const Structure& const_reference;

    /**
     * Get the number of immediate subfields in the structure/
     * @return The number of fields.
     */
    std::size_t getNumberFields() const {return fieldNames.size();}
    /**
     * Get the field for the specified fieldName.
     * @param fieldName The name of the field to get;
     * @return The introspection interface.
     * This will hold a null pointer if the field is not in the structure.
     */
    FieldConstPtr getField(String const &fieldName) const;
    /**
     * Get the field for the specified fieldName.
     * @param fieldName The index of the field to get;
     * @return The introspection interface.
     * This will hold a null pointer if the field is not in the structure.
     */
    FieldConstPtr getField(std::size_t index) const {return fields[index];}
    /**
     * Get the field index for the specified fieldName.
     * @return The introspection interface.
     * This will be -1 if the field is not in the structure.
     */
    std::size_t getFieldIndex(String const &fieldName) const;
    /**
     * Get the fields in the structure.
     * @return The array of fields.
     */
    FieldConstPtrArray const & getFields() const {return fields;}
    /**
     * Get the names of the fields in the structure.
     * @return The array of fieldNames.
     */
    StringArray const & getFieldNames() const {return fieldNames;}
    void renameField(std::size_t fieldIndex,String const & newName)
        {fieldNames[fieldIndex] = newName;}
    /**
     * Get the name of the field with the specified index;
     * @param fieldIndex The index of the desired field.
     * @return The fieldName.
     */
    String getFieldName(std::size_t fieldIndex){return fieldNames[fieldIndex];}
    /**
     * Convert the structure to a string and add it to builder.
     * @param  builder The string builder.
     */
    virtual void toString(StringBuilder buf) const{toString(buf,0);}
    /**
     * Convert the structure to a string and add it to builder.
     * @param  builder The string builder.
     * @param  indentLevel The number of blanks at the beginning of new lines.
     */
    virtual void toString(StringBuilder buf,int indentLevel) const;
    
    virtual String getID() const;

    virtual void serialize(ByteBuffer *buffer, SerializableControl *control) const;
    virtual void deserialize(ByteBuffer *buffer, DeserializableControl *control);
    
protected:
   Structure(StringArray const & fieldNames, FieldConstPtrArray const & fields, String const & id = DEFAULT_ID);
private:
    void toStringCommon(StringBuilder buf,int indentLevel) const;
    StringArray fieldNames;
    FieldConstPtrArray fields;
    String id;
   friend class FieldCreate;
};

/**
 * This is a singlton class for creating introspection interfaces.
 */
class FieldCreate;
typedef std::tr1::shared_ptr<FieldCreate> FieldCreatePtr;

class epicsShareClass FieldCreate {
public:
     static FieldCreatePtr getFieldCreate();
    /**
     * Create a {@code ScalarField}.
     * @param scalarType The scalar type.
     * @return a {@code Scalar} interface for the newly created object.
     * @throws An {@code IllegalArgumentException} if an illegal type is specified.
     */
    ScalarConstPtr  createScalar(ScalarType scalarType) const;
    /**
     * Create an {@code Array} field.
     * @param elementType The {@code scalarType} for array elements
     * @return An {@code Array} Interface for the newly created object.
     */
    ScalarArrayConstPtr createScalarArray(ScalarType elementType) const;
     /**
      * Create an {@code Array} field that is has element type <i>Structure</i>
      * @param fieldName The field name
      * @param elementStructure The {@code Structure} for each array element.
      * @return An {@code Array} Interface for the newly created object.
      */
    StructureArrayConstPtr createStructureArray(StructureConstPtr const & structure) const;
    /**
     * Create a {@code Structure} field.
     * @param fieldNames The array of {@code fieldNames} for the structure.
     * @param fields The array of {@code fields} for the structure.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr createStructure (
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
    /**
     * Create a {@code Structure} field with identification string.
     * @param id The identification string for the structure.
     * @param fieldNames The array of {@code fieldNames} for the structure.
     * @param fields The array of {@code fields} for the structure.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr createStructure (
    	String const & id,
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
    /**
     * Append a field to a structure.
     * @param structure The structure to which the field is appended.
     * @param fieldName The name of the field.
     * @param field The field.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr appendField(
        StructureConstPtr const & structure,
        String const & fieldName, FieldConstPtr const & field) const;
    /**
     * Append fields to a structure.
     * @param structure The structure to which the fields appended.
     * @param fieldName The names of the fields.
     * @param field The fields.
     * @return a {@code Structure} interface for the newly created object.
     */
    StructureConstPtr appendFields(
        StructureConstPtr const & structure,
        StringArray const & fieldNames,
        FieldConstPtrArray const & fields) const;
    /**
     * Deserialize {@code Field} instance from given byte buffer.
     * @param buffer Buffer containing serialized {@code Field} instance. 
     * @param control Deserialization control instance.
     * @return a deserialized {@code Field} instance.
     */
    FieldConstPtr deserialize(ByteBuffer* buffer, DeserializableControl* control) const;
        
private:
   FieldCreate();
};

/**
 * Get the single class that implemnents FieldCreate,
 * @param The fieldCreate factory.
 */
epicsShareExtern FieldCreatePtr getFieldCreate();

}}
#endif  /* PVINTROSPECT_H */