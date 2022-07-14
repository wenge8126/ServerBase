
namespace Logic
{
    abstract class tFieldInfoFactory
    {
        public abstract FIELD_TYPE getType();

        public abstract tFieldInfo NewFieldInfo();
    };
    //-------------------------------------------------------------------------

    class IntFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_INT; }

        public override tFieldInfo NewFieldInfo()
        {
            return new IntFieldInfo();
        }
    };
    //-------------------------------------------------------------------------

    class ShortFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_SHORT; }

        public override tFieldInfo NewFieldInfo()
        {
            return new ShortFieldInfo();
        }
    };
    //-------------------------------------------------------------------------

    class BoolFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_BOOL; }

        public override tFieldInfo NewFieldInfo()
        {
            return new BoolFieldInfo();
        }
    };

    //--------------------------------------------------------------------------------

    class UINT64FieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_UINT64; }

        public override tFieldInfo NewFieldInfo()
        {
            return new UINT64FieldInfo();
        }
    };
    //--------------------------------------------------------------------------------

    class NiceDataFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_NICEDATA; }

        public override tFieldInfo NewFieldInfo()
        {
            return new NiceDataFieldInfo();
        }
    };
    //--------------------------------------------------------------------------------

    class FloatFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_FLOAT; }

        public override tFieldInfo NewFieldInfo()
        {
            return new FloatFieldInfo();
        }
    };
    //--------------------------------------------------------------------------------

    class DoubleFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_DOUBLE; }

        public override tFieldInfo NewFieldInfo()
        {
            return new DoubleFieldInfo();
        }
    };
    //--------------------------------------------------------------------------------

    class StringFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_STRING; }

        public override tFieldInfo NewFieldInfo()
        {
            return new StringFieldInfo();
        }
    };
    //--------------------------------------------------------------------------------

    class ByteFieldInfoFactory : tFieldInfoFactory
    {
        public override FIELD_TYPE getType() { return FIELD_TYPE.FIELD_BYTE; }

        public override tFieldInfo NewFieldInfo()
        {
            return new ByteFieldInfo();
        }
    };

    //--------------------------------------------------------------------------------


}
