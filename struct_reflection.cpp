/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Bryn Murrell $
   ======================================================================== */
#include "blib_utils.h"
#include <stdio.h>
#include <malloc.h>
#include <string>
#include "Parsing.h"
#include "len_string.h"
#include "DebugFileIO.cpp"
#include "ComponentType.h"
#include <unordered_map>

#include "metaprogramming.h"

len_string reflection_func_name;
len_string f_name_no_dirs;
flocal void append_struct_reflection_function(len_string* builder,
                                              len_string struct_name,
                                              std::vector<len_string> field_names)
{
    char buf[512];
    sprintf(buf, "\n%s_STRUCT_REFLECTION_FUNC(%s_reflection_func)\n{\n",f_name_no_dirs.str, struct_name.str);
    append_to_len_string(builder, buf);


    LOOP(i, field_names.size())
    {
        
        append_to_len_string(builder, "\t");
        if (i != 0)
        {
            append_to_len_string(builder, "else ");
        }
        sprintf(buf, "if(streq(\"%s\", field_name, %d))\n",field_names[i].str, field_names[i].string_len);
        append_to_len_string(builder, buf);
        append_to_len_string(builder, "\t{\n");
        sprintf(buf, "\t\t return offsetof(%s, %s);\n", struct_name.str, field_names[i].str);
        append_to_len_string(builder, buf);
        append_to_len_string(builder, "\t}\n");
        
        
    }
    append_to_len_string(builder, "\treturn -1;\n");
    append_to_len_string(builder, "}\n");
}

int main(u32 argc, char** argv)
{
    if (argc >= 2 && is_cpp_or_h_file(argv[1]))
    {
        printf("cpp or h file passed\n");
    }
    else
    {
        printf("no cpp or h file passed\n");
    }
    u64 length = 0;
    char* in_file_name = argv[1];
    char* full_file_str = read_entire_file_text(in_file_name, &length);
    u32 id = 0;

    u32 last_idx = 0;
    u32 ctr = 0;
    char* cpy = in_file_name;
    do 
    {
        if (*cpy == '\\' || *cpy == '/')
        {
            last_idx = ctr;
        }
        ctr++;
    } while(*cpy++);

    f_name_no_dirs = l_string(&in_file_name[last_idx+1]);
    strip_extension(&f_name_no_dirs);
    
    len_string builder = l_string(1);
    
    #define STRUCT "struct"
    Token tok_struct = token(TOKEN_IDENTIFIER, strlen(STRUCT), STRUCT);

    char buf[512];

    sprintf(buf, "%s_struct_reflection_func", f_name_no_dirs.str);
    reflection_func_name = l_string(buf);
    
    sprintf(buf, "\n#define %s_STRUCT_REFLECTION_FUNC(name) u64 name(char* field_name)\ntypedef %s_STRUCT_REFLECTION_FUNC(%s);\n\n",
            f_name_no_dirs.str, f_name_no_dirs.str, reflection_func_name.str);
    append_to_len_string(&builder, buf);

    
    while(find_next_token_in_file(&full_file_str, tok_struct))
    {
        i32 brace_counter = 0;
        Tokenizer tok = {};
        tok.at = full_file_str;
        std::vector<len_string> field_names;
        Token struct_name = getToken(&tok);
        Token should_be_brace = getToken(&tok);
        if (should_be_brace.type == TOKEN_BRACE_OPEN)
        {
            while(true)
            {
                Token t = getToken(&tok);
                if (streq(t.text, "std", 3))
                {
                    eat_to_next_whitespace(&tok);
                }
                if (t.type == TOKEN_IDENTIFIER)
                {
                    t = getToken(&tok);
            
                    if (t == token(TOKEN_BRACKET_OPEN, 1, "["))
                    {
                        find_next_token(&tok, token(TOKEN_BRACKET_CLOSE, 1, "]"));
                    }            
                    if (t == token(TOKEN_BRACKET_CLOSE, 1, "]"))
                    {
                        find_next_token(&tok, token(TOKEN_BRACKET_CLOSE, 1, "]"));
                    }
                    while(t.type == TOKEN_ASTERISK)
                    {
                        t = getToken(&tok);
                    }
            
                    field_names.push_back(l_string(t.text, t.length));
                    while (peek_tok(tok) == token(TOKEN_BRACKET_OPEN, 1, "["))
                    {
                        find_next_token(&tok, token(TOKEN_BRACKET_CLOSE, 1, "]"));
                    }
                }
                else if (t.type == TOKEN_BRACE_CLOSE)
                {
                    if (brace_counter == 0)
                    {
                        break;
                    }
                    brace_counter += update_brace_counter(t);
                }
                else if (t.type == TOKEN_BRACE_OPEN)
                {
                    brace_counter += update_brace_counter(t);   
                }
                else if (t.type == TOKEN_ASSIGNMENT)
                {
                    t = getToken(&tok);
                }
            
            }
            append_struct_reflection_function(&builder, l_string(struct_name.text, struct_name.length), field_names);
        }
    }

    len_string out_file_name = l_string(in_file_name);
    strip_extension(&out_file_name);
    append_to_len_string(&out_file_name, "_struct_reflection.inl");
    FILE* out = fopen(out_file_name.str, "wb");
    fwrite(builder.str, 1, builder.string_len, out);
    fclose(out);
}
