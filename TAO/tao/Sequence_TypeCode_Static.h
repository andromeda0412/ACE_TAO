// -*- C++ -*-

//=============================================================================
/**
 *  @file    Sequence_TypeCode_Static.h
 *
 *  $Id$
 *
 *  Header file for static @c tk_sequence and @c tk_array
 *  @c CORBA::TypeCodes.
 *
 *  @author Ossama Othman <ossama@dre.vanderbilt.edu>
 */
//=============================================================================

#ifndef TAO_SEQUENCE_TYPECODE_STATIC_H
#define TAO_SEQUENCE_TYPECODE_STATIC_H

#include /**/ "ace/pre.h"

#include "tao/TypeCode.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/Null_RefCount_Policy.h"


namespace TAO
{
  namespace TypeCode
  {
    template <typename TypeCodeType, class RefCountPolicy> class Sequence;

    /**
     * @class Sequence
     *
     * @brief @c CORBA::TypeCode implementation for OMG IDL
     *        @c sequence and @c array types.
     *
     * This class implements a @c CORBA::TypeCode for OMG IDL
     * @c sequence and array types.
     */
    template<>
    class TAO_Export Sequence<CORBA::TypeCode_ptr const *,
                              TAO::Null_RefCount_Policy>
      : public CORBA::TypeCode
      , private TAO::Null_RefCount_Policy
    {
    public:

      /// Constructor.
      Sequence (CORBA::TCKind kind,
                CORBA::TypeCode_ptr const * content_type,
                CORBA::ULong length);

      /**
       * @name TAO-specific @c CORBA::TypeCode Methods
       *
       * Methods required by TAO's implementation of the
       * @c CORBA::TypeCode class.
       *
       * @see @c CORBA::TypeCode
       */
      //@{
      virtual bool tao_marshal (TAO_OutputCDR & cdr) const;
      virtual void tao_duplicate (void);
      virtual void tao_release (void);
      //@}

    protected:

      /**
       * @name @c TAO CORBA::TypeCode Template Methods
       *
       * @c CORBA::TypeCode template methods specific to @c tk_sequence
       * @c TypeCodes.
       *
       * @see @c CORBA::TypeCode
       */
      //@{
      virtual CORBA::Boolean equal_i (CORBA::TypeCode_ptr tc
                                      ACE_ENV_ARG_DECL) const;
      virtual CORBA::Boolean equivalent_i (CORBA::TypeCode_ptr tc
                                           ACE_ENV_ARG_DECL) const;
      virtual CORBA::TypeCode_ptr get_compact_typecode_i (
        ACE_ENV_SINGLE_ARG_DECL) const;
      virtual CORBA::ULong length_i (ACE_ENV_SINGLE_ARG_DECL) const;
      virtual CORBA::TypeCode_ptr content_type_i (
        ACE_ENV_SINGLE_ARG_DECL) const;

    private:

      /// Element type of the sequence.
      /**
       * A pointer to the @c CORBA::TypeCode_ptr rather than the
       * @c CORBA::TypeCode_ptr itself is stored since that address is
       * well-defined.  We may not know the value of the @c
       * CORBA::TypeCode_ptr when creating this @c Field statically at
       * compile-time, hence the indirection.
       *
       * @note This @c TypeCode is released upon destruction of this
       *       @c TypeCode::Sequence.
       */
      CORBA::TypeCode_ptr const * const content_type_;

      /// Length of the @c sequence or array.  A length of zero
      /// indicates an unbounded @c sequence.
      CORBA::ULong const length_;

    };

  }  // End namespace TypeCode
}  // End namespace TAO


#ifdef __ACE_INLINE__
# include "tao/Sequence_TypeCode_Static.inl"
#endif  /* __ACE_INLINE__ */

#include /**/ "ace/post.h"

#endif /* TAO_SEQUENCE_TYPECODE_STATIC_H */
