#ifndef INC_PARSERTYPESTIE_HPP_3ECC7D21_D8F6_48A1_A1D8_38F366319B3F
#define INC_PARSERTYPESTIE_HPP_3ECC7D21_D8F6_48A1_A1D8_38F366319B3F

#include "ParserTypes.hpp"
namespace xcomidl
{

    template <class Impl>
    class IParserTie : public IParserRaw
    {
    public:
        static xcom::IUnknownRaw* queryInterface__call(void* ptr, ::xcom::Environment* __exc_info, xcom::GUID const* iid)
        {
            try {
            return static_cast<Impl*>(static_cast<IParserTie<Impl>*>(ptr))->queryInterface(*(xcom::GUID*)iid).detach();
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            return xcom::IUnknown().detach();
            
        }
        
        static xcom::GUID getInterfaceId__call(void*, ::xcom::Environment*)
        {
            return IParser::thisInterfaceId();
        }
        
        static xcom::Int addRef__call(void* ptr, ::xcom::Environment* __exc_info)
        {
            try {
            return static_cast<Impl*>(static_cast<IParserTie<Impl>*>(ptr))->addRef();
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            return xcom::Int();
            
        }
        
        static xcom::Int release__call(void* ptr, ::xcom::Environment* __exc_info)
        {
            try {
            return static_cast<Impl*>(static_cast<IParserTie<Impl>*>(ptr))->release();
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            return xcom::Int();
            
        }
        
        static xcom::Bool parse__call(void* ptr, ::xcom::Environment* __exc_info, xcom::StringSeq::RawType const* includePaths, const xcom::Char* idlFile, xcomidl::TypeSeq::RawType* types, xcomidl::HintSeq::RawType* hints, xcom::StringSeq::RawType* messages)
        {
            try {
            return static_cast<Impl*>(static_cast<IParserTie<Impl>*>(ptr))->parse(*(xcom::StringSeq*)includePaths, idlFile, *(xcomidl::TypeSeq*)types, *(xcomidl::HintSeq*)hints, *(xcom::StringSeq*)messages);
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            return xcom::Bool();
            
        }
        
        
        
        IParserTie()
        {
            vptr_ = &IParserTieVtbl;
        }
    
    private:
        static IParserVtbl IParserTieVtbl;
    };
    
    template <class Impl>
    IParserVtbl IParserTie<Impl>::IParserTieVtbl =
    {
        &IParserTie<Impl>::queryInterface__call,
        &IParserTie<Impl>::getInterfaceId__call,
        &IParserTie<Impl>::addRef__call,
        &IParserTie<Impl>::release__call,
        &IParserTie<Impl>::parse__call,
        
    };
    
    template <class Impl>
    class ICodeGenTie : public ICodeGenRaw
    {
    public:
        static xcom::IUnknownRaw* queryInterface__call(void* ptr, ::xcom::Environment* __exc_info, xcom::GUID const* iid)
        {
            try {
            return static_cast<Impl*>(static_cast<ICodeGenTie<Impl>*>(ptr))->queryInterface(*(xcom::GUID*)iid).detach();
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            return xcom::IUnknown().detach();
            
        }
        
        static xcom::GUID getInterfaceId__call(void*, ::xcom::Environment*)
        {
            return ICodeGen::thisInterfaceId();
        }
        
        static xcom::Int addRef__call(void* ptr, ::xcom::Environment* __exc_info)
        {
            try {
            return static_cast<Impl*>(static_cast<ICodeGenTie<Impl>*>(ptr))->addRef();
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            return xcom::Int();
            
        }
        
        static xcom::Int release__call(void* ptr, ::xcom::Environment* __exc_info)
        {
            try {
            return static_cast<Impl*>(static_cast<ICodeGenTie<Impl>*>(ptr))->release();
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            return xcom::Int();
            
        }
        
        static void generate__call(void* ptr, ::xcom::Environment* __exc_info, xcomidl::TypeSeq::RawType const* types, xcomidl::HintSeq::RawType const* hints, const xcom::Char* idlFileName, xcom::StringSeq::RawType const* options)
        {
            try
            {
                static_cast<Impl*>(static_cast<ICodeGenTie<Impl>*>(ptr))->generate(*(xcomidl::TypeSeq*)types, *(xcomidl::HintSeq*)hints, idlFileName, *(xcom::StringSeq*)options);
                
            } catch(xcom::UserExc& ue) { ue.detach(__exc_info); }
            }
            
        
        
        ICodeGenTie()
        {
            vptr_ = &ICodeGenTieVtbl;
        }
    
    private:
        static ICodeGenVtbl ICodeGenTieVtbl;
    };
    
    template <class Impl>
    ICodeGenVtbl ICodeGenTie<Impl>::ICodeGenTieVtbl =
    {
        &ICodeGenTie<Impl>::queryInterface__call,
        &ICodeGenTie<Impl>::getInterfaceId__call,
        &ICodeGenTie<Impl>::addRef__call,
        &ICodeGenTie<Impl>::release__call,
        &ICodeGenTie<Impl>::generate__call,
        
    };
    
}

#endif
