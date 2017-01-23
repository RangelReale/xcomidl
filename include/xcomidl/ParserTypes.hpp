#ifndef INC_PARSERTYPES_HPP_29C8E9EC_DF5A_4BFA_82BC_B4BF744B94E3
#define INC_PARSERTYPES_HPP_29C8E9EC_DF5A_4BFA_82BC_B4BF744B94E3

#include <xcom/Types.hpp>

#include <xcom/metadata/MDTypes.hpp>
namespace xcomidl
{
    namespace CodeGenHint
    {
        enum type
        {
            GenImport,
            GenType,
            GenForward,
            EnterNamespace,
            LeaveNamespace,
            
        };
    }
    typedef CodeGenHint::type CodeGenHintEnum;
    
    
    struct HintData
    {
        xcom::Int type;
        xcom::Char* parameter;
        
    };
    
    struct Hint
    {
        xcom::Int type;
        xcom::String parameter;
        
        typedef HintData RawType;
        RawType detach()
        {
            RawType result;
            
            result.type = type;
            result.parameter = parameter.detach();
            
            
            return result;
        };
        
        static Hint adopt(RawType const& raw)
        {
            Hint result;
            ::memcpy(&result, &raw, sizeof(RawType));
            return result;
        }
        
    };
    
    class HintSeq : public xcom::SequenceBase<xcomidl::Hint, xcomidl::HintData>
    {
    public:
        static HintSeq adopt(RawType const& src)
        {
            HintSeq result;
            ::memcpy(&result, &src, sizeof(RawType));
            return result;
        }
        
        HintSeq() {}
        explicit HintSeq(xcom::Int size)
        : xcom::SequenceBase<xcomidl::Hint, xcomidl::HintData>(size)
        {
        }
    };
    
    class TypeSeq : public xcom::SequenceBase<xcom::metadata::IType, xcom::metadata::ITypeRaw*>
    {
    public:
        static TypeSeq adopt(RawType const& src)
        {
            TypeSeq result;
            ::memcpy(&result, &src, sizeof(RawType));
            return result;
        }
        
        TypeSeq() {}
        explicit TypeSeq(xcom::Int size)
        : xcom::SequenceBase<xcom::metadata::IType, xcom::metadata::ITypeRaw*>(size)
        {
        }
    };
    
    struct IParserRaw : public xcom::IUnknownRaw
    {
    };
    struct IParserVtbl
    {
        xcom::IUnknownRaw* (*queryInterface)(void*, xcom::Environment*, xcom::GUID const* iid);
        xcom::GUID (*getInterfaceId)(void*, xcom::Environment*);
        xcom::Int (*addRef)(void*, xcom::Environment*);
        xcom::Int (*release)(void*, xcom::Environment*);
        xcom::Bool (*parse)(void*, xcom::Environment*, xcom::StringSeq::RawType const* includePaths, const xcom::Char* idlFile, xcomidl::TypeSeq::RawType* types, xcomidl::HintSeq::RawType* hints, xcom::StringSeq::RawType* messages);
        
    };
    template<typename Impl> class IParserTie;
    class IParser : public xcom::IUnknown
    {
    public:
        typedef IParserRaw* RawType;
        typedef xcom::IUnknown ParentClass;
        template<typename T>
        struct Tie { typedef IParserTie<T> type; };
        IParser() {}
        IParser(IParserRaw* ptr) : xcom::IUnknown((xcom::IUnknownRaw*)ptr) {}
        xcom::Bool parse(xcom::StringSeq const& includePaths, const xcom::Char* idlFile, xcomidl::TypeSeq& types, xcomidl::HintSeq& hints, xcom::StringSeq& messages) const;
        
        static IParser adopt(IParserRaw* src)
        {
            return IParser(src);
        }
        
        IParserRaw* detach()
        {
            IParserRaw* result = (IParserRaw*)ptr_;
            ptr_ = 0;
            return result;
        }
        
        static inline xcom::GUID const& thisInterfaceId()
        {
            static const xcom::GUID id =
            {
                1763709045, -17175, 18700,
                {0xb0, 0x03, 0x8c, 0x52, 0x74, 0xc8, 0x36, 0x4b}
            };
            
            return id;
        }
        
    };
    
    struct ICodeGenRaw : public xcom::IUnknownRaw
    {
    };
    struct ICodeGenVtbl
    {
        xcom::IUnknownRaw* (*queryInterface)(void*, xcom::Environment*, xcom::GUID const* iid);
        xcom::GUID (*getInterfaceId)(void*, xcom::Environment*);
        xcom::Int (*addRef)(void*, xcom::Environment*);
        xcom::Int (*release)(void*, xcom::Environment*);
        void (*generate)(void*, xcom::Environment*, xcomidl::TypeSeq::RawType const* types, xcomidl::HintSeq::RawType const* hints, const xcom::Char* idlFileName, xcom::StringSeq::RawType const* options);
        
    };
    template<typename Impl> class ICodeGenTie;
    class ICodeGen : public xcom::IUnknown
    {
    public:
        typedef ICodeGenRaw* RawType;
        typedef xcom::IUnknown ParentClass;
        template<typename T>
        struct Tie { typedef ICodeGenTie<T> type; };
        ICodeGen() {}
        ICodeGen(ICodeGenRaw* ptr) : xcom::IUnknown((xcom::IUnknownRaw*)ptr) {}
        void generate(xcomidl::TypeSeq const& types, xcomidl::HintSeq const& hints, const xcom::Char* idlFileName, xcom::StringSeq const& options) const;
        
        static ICodeGen adopt(ICodeGenRaw* src)
        {
            return ICodeGen(src);
        }
        
        ICodeGenRaw* detach()
        {
            ICodeGenRaw* result = (ICodeGenRaw*)ptr_;
            ptr_ = 0;
            return result;
        }
        
        static inline xcom::GUID const& thisInterfaceId()
        {
            static const xcom::GUID id =
            {
                -1632691297, -19031, 18308,
                {0x8a, 0x8b, 0x41, 0xbe, 0xad, 0x9d, 0x47, 0xaa}
            };
            
            return id;
        }
        
    };
    
}
namespace xcomidl
{
    inline xcom::Bool IParser::parse(xcom::StringSeq const& includePaths, const xcom::Char* idlFile, xcomidl::TypeSeq& types, xcomidl::HintSeq& hints, xcom::StringSeq& messages) const
    {
        xcom::Environment __exc_info;
        xcom::Bool result(static_cast<IParserVtbl*>(static_cast<IParserRaw*>(ptr_)->vptr_)->parse(ptr_, &__exc_info, (xcom::StringSeq::RawType const*)&includePaths, idlFile, (xcomidl::TypeSeq::RawType*)&types, (xcomidl::HintSeq::RawType*)&hints, (xcom::StringSeq::RawType*)&messages));
        if(__exc_info.exception) xcomFindAndThrow(&__exc_info);
        
        return result;
    }
    
    inline void ICodeGen::generate(xcomidl::TypeSeq const& types, xcomidl::HintSeq const& hints, const xcom::Char* idlFileName, xcom::StringSeq const& options) const
    {
        xcom::Environment __exc_info;
        static_cast<ICodeGenVtbl*>(static_cast<ICodeGenRaw*>(ptr_)->vptr_)->generate(ptr_, &__exc_info, (xcomidl::TypeSeq::RawType const*)&types, (xcomidl::HintSeq::RawType const*)&hints, idlFileName, (xcom::StringSeq::RawType const*)&options);
    if(__exc_info.exception) xcomFindAndThrow(&__exc_info);
    
    }
    
}
#include <xcom/MDHelper.hpp>
namespace xcom
{

    template<> struct TypeDesc<xcomidl::IParser>
    {
        static void addSelf(IUnknownSeq& types);
    };
    
    template<> struct TypeDesc<xcomidl::ICodeGen>
    {
        static void addSelf(IUnknownSeq& types);
    };
    
    template <>
    struct TypeDesc<xcomidl::CodeGenHintEnum>
    {
        static void addSelf(IUnknownSeq& types)
        {
            if(!typeExists(types, "xcomidl.CodeGenHint"))
            {
                Char const* elements[5] = { "GenImport", "GenType", "GenForward", "EnterNamespace", "LeaveNamespace",  };
                addType(types, xcomCreateEnumMD("xcomidl.CodeGenHint", elements, 5));
            }
        }
    };
    template <>
    struct TypeDesc<xcomidl::Hint>
    {
        static void addSelf(IUnknownSeq& types)
        {
            if(!typeExists(types, "xcomidl.Hint"))
            {
                IUnknownRaw* mtypes[2] =
                {
                    rawFindOrReg(types, "xcomidl.CodeGenHint", &TypeDesc<xcomidl::CodeGenHintEnum>::addSelf),
                    rawFindMetadata(types, "string"),
                    
                };
                
                const Char* mnames[2] =
                {
                    "type",
                    "parameter",
                    
                };
                
                Int moffsets[2] =
                {
                    offsetof(xcomidl::HintData, type),
                    offsetof(xcomidl::HintData, parameter),
                    
                };
                
                addType(types, xcomCreateStructMD("xcomidl.Hint", sizeof(xcomidl::HintData), 2, mtypes, mnames, moffsets));
            }
        }
    };
    
    template <>
    struct TypeDesc<xcomidl::HintSeq>
    {
        static void addSelf(IUnknownSeq& types)
        {
            if(!typeExists(types, "xcomidl.HintSeq"))
            {
                addType(types, xcomCreateSequenceMD("xcomidl.HintSeq", rawFindOrReg(types, "xcomidl.Hint", &TypeDesc<xcomidl::Hint>::addSelf)));
            }
        }
    };
    
    template <>
    struct TypeDesc<xcomidl::TypeSeq>
    {
        static void addSelf(IUnknownSeq& types)
        {
            if(!typeExists(types, "xcomidl.TypeSeq"))
            {
                addType(types, xcomCreateSequenceMD("xcomidl.TypeSeq", rawFindOrReg(types, "xcom.metadata.IType", &TypeDesc<xcom::metadata::IType>::addSelf)));
            }
        }
    };
    
    inline void TypeDesc<xcomidl::IParser>::addSelf(IUnknownSeq& types)
    {
        if(!typeExists(types, "xcomidl.IParser"))
        {
            void* cookie;
            IUnknown base(findOrRegister(types, "xcom.IUnknown", &TypeDesc<xcom::IUnknown>::addSelf));
            Char const* pnames[6];
            IUnknownRaw* ptypes[6];
            Int pmodes[6];
            types.push_back(xcomCreateInterfaceMD("xcomidl.IParser", &xcomidl::IParser::thisInterfaceId(), base.detach(), &cookie));
            
            pnames[0] = "includePaths";
            pnames[1] = "idlFile";
            pnames[2] = "types";
            pnames[3] = "hints";
            pnames[4] = "messages";
            
            ptypes[0] = rawFindOrReg(types, "xcom.StringSeq", &TypeDesc<xcom::StringSeq>::addSelf);
            ptypes[1] = rawFindMetadata(types, "string");
            ptypes[2] = rawFindOrReg(types, "xcomidl.TypeSeq", &TypeDesc<xcomidl::TypeSeq>::addSelf);
            ptypes[3] = rawFindOrReg(types, "xcomidl.HintSeq", &TypeDesc<xcomidl::HintSeq>::addSelf);
            ptypes[4] = rawFindOrReg(types, "xcom.StringSeq", &TypeDesc<xcom::StringSeq>::addSelf);
            
            pmodes[0] = 0;
            pmodes[1] = 0;
            pmodes[2] = 1;
            pmodes[3] = 1;
            pmodes[4] = 1;
            
            xcomAddMethodToItf(cookie, "parse", rawFindMetadata(types, "bool"), 5, pmodes, ptypes, pnames);
            
        }
    }
    
    inline void TypeDesc<xcomidl::ICodeGen>::addSelf(IUnknownSeq& types)
    {
        if(!typeExists(types, "xcomidl.ICodeGen"))
        {
            void* cookie;
            IUnknown base(findOrRegister(types, "xcom.IUnknown", &TypeDesc<xcom::IUnknown>::addSelf));
            Char const* pnames[5];
            IUnknownRaw* ptypes[5];
            Int pmodes[5];
            types.push_back(xcomCreateInterfaceMD("xcomidl.ICodeGen", &xcomidl::ICodeGen::thisInterfaceId(), base.detach(), &cookie));
            
            pnames[0] = "types";
            pnames[1] = "hints";
            pnames[2] = "idlFileName";
            pnames[3] = "options";
            
            ptypes[0] = rawFindOrReg(types, "xcomidl.TypeSeq", &TypeDesc<xcomidl::TypeSeq>::addSelf);
            ptypes[1] = rawFindOrReg(types, "xcomidl.HintSeq", &TypeDesc<xcomidl::HintSeq>::addSelf);
            ptypes[2] = rawFindMetadata(types, "string");
            ptypes[3] = rawFindOrReg(types, "xcom.StringSeq", &TypeDesc<xcom::StringSeq>::addSelf);
            
            pmodes[0] = 0;
            pmodes[1] = 0;
            pmodes[2] = 0;
            pmodes[3] = 0;
            
            xcomAddMethodToItf(cookie, "generate", rawFindMetadata(types, "void"), 4, pmodes, ptypes, pnames);
            
        }
    }
    
} // namespace xcom

#include <xcom/ExcHelper.hpp>

namespace xcomidl
{

}

#endif
