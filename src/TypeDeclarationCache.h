#ifndef TYPETESTERDYNTRACER_TYPE_DECLARATION_CACHE_H
#define TYPETESTERDYNTRACER_TYPE_DECLARATION_CACHE_H

#include <filesystem>
#include <memory>
#include <tastr/ast/ast.hpp>
#include <tastr/parser/parser.hpp>

namespace fs = std::filesystem;

class TypeDeclarationCache {
  public:
    explicit TypeDeclarationCache(const fs::path& type_declaration_dirpath)
        : type_declaration_dirpath_(fs::canonical(type_declaration_dirpath)) {
        if (!fs::is_directory(type_declaration_dirpath_)) {
            dyntrace_log_error("Unable to find type declaration directory '%s'",
                               type_declaration_dirpath_.c_str());
        }
    }

    ~TypeDeclarationCache() = default;

    tastr::ast::FunctionTypeNode*
    get_function_type(const std::string& package_name,
                      const std::string& function_name) {
        const fs::path package_path = type_declaration_dirpath_ / package_name;

        auto package = packages_.find(package_name);

        if (package == packages_.end()) {
            package = packages_
                          .insert({package_name,
                                   import_type_declarations(package_path)})
                          .first;
        }

        auto type_iter = package->second.find(function_name);
        if (type_iter == package->second.end()) {
            return nullptr;
        }
        tastr::ast::TypeNode* node = type_iter->second.get();
        if (node->is_function_type_node()) {
            return static_cast<tastr::ast::FunctionTypeNode*>(node);
        }

        return nullptr;
    }

    std::unordered_map<std::string, tastr::ast::TypeNodeUPtr>
    import_type_declarations(
        const fs::path& package_type_declaration_filepath) {
        tastr::parser::ParseResult result(
            tastr::parser::parse_file(package_type_declaration_filepath));

        std::unordered_map<std::string, tastr::ast::TypeNodeUPtr> package_map;

        for (const std::unique_ptr<tastr::ast::TypeDeclarationNode>& decl:
             result.get_top_level_node()->get_type_declarations()) {
            std::string name(decl->get_identifier().get_name());
            package_map.insert(std::make_pair(name, decl->get_type().clone()));
        }
        return package_map;
    }

  private:
    std::unordered_map<
        std::string,
        std::unordered_map<std::string, tastr::ast::TypeNodeUPtr>>
        packages_;

    fs::path type_declaration_dirpath_;
};

#endif /* TYPETESTERDYNTRACER_TYPE_DECLARATION_CACHE_H */
