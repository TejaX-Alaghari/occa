#include <occa/lang/statement.hpp>
#include <occa/lang/variable.hpp>
#include <occa/lang/qualifier.hpp>
#include <occa/lang/builtins/types.hpp>
#include <occa/lang/transforms/builtins/restrict.hpp>

namespace occa {
  namespace lang {
    namespace transforms {
      restrict::restrict(const qualifier_t &restrictQualifier_) :
        restrictQualifier(restrictQualifier_) {
        validStatementTypes = (statementType::functionDecl |
                               statementType::function);
      }

      statement_t* restrict::transformStatement(statement_t &smnt) {
        function_t &func = (
          (smnt.type() & statementType::function)
          ? smnt.to<functionStatement>().function
          : smnt.to<functionDeclStatement>().function
        );

        const int argc = (int) func.args.size();
        for (int i = 0; i < argc; ++i) {
          variable_t *arg = func.args[i];
          if (arg && arg->hasAttribute("restrict")) {
            const int pointerCount = (int) arg->vartype.pointers.size();
            if (pointerCount) {
              arg->vartype.pointers[pointerCount - 1] += restrictQualifier;
            } else {
              arg->attributes["restrict"].printError(
                "[@restrict] can only be applied to pointer function arguments"
              );
              return NULL;
            }
          }
        }

        return &smnt;
      }

      bool applyRestrictTransforms(statement_t &smnt,
                                   const qualifier_t &restrictQualifier) {
        restrict restrictTransform(restrictQualifier);
        return restrictTransform.statementTransform::apply(smnt);
      }
    }
  }
}
