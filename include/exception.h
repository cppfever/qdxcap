#pragma once

#define EXCEPTION_BUFFER_SIZE 64

#include <cstring>
#include <string>
#include <string_view>


/**
    This one class uses for all exceptions.
    With ExceptionInfo macro you can get place where exception was caught.
    Information placed in protected data members.
    Additional information can be added via shift left operator.
    This class no uses heap allocation , but instead six arrays of char's with EXCEPTION_BUFFER_SIZE size,
    and 'C' functions like 'strncpy', 'strncat' to manage this arrays.
*/
class Exception
{
public:

    /**
        This constructor create empty exception object.
        Additional user defined information can be added later.
        Information about place don't save.
    */
    Exception() : Exception("", "", 0, "")
    {}

    /**
        This constructor create Exception object with user defined information.
        Information about place don't save.
    */
    Exception(std::string_view text) : Exception(text, "", 0, "")
    {}

    /**
        This constructor save information about place(file name, line of code, function name)
        where exception was caught.
        Additional user defined information can be added later.
    */
    Exception(std::string_view file, int line, std::string_view func) : Exception("", file, line, func)
    {}

    /**
        This constructor save information about place(file name, line of code, function name)
        where exception was caught with user defined information.
        Additional user defined information can be added later.
    */
    Exception(std::string_view text, std::string_view file, int line, std::string_view func)
    {
        //Copy text to buffers
        std::strncpy(m_text, text.begin(), EXCEPTION_BUFFER_SIZE - 1);
        std::strncpy(m_file, file.begin(), EXCEPTION_BUFFER_SIZE - 1);
        std::strncpy(m_func, func.begin(), EXCEPTION_BUFFER_SIZE - 1);

        //If line is not null, then transform it to string in s_buffer
        //and copy to m_line
        if(line != 0)
        {
            std::snprintf(s_buffer, EXCEPTION_BUFFER_SIZE - 1, "%d", line);
            std::strncpy(m_line, s_buffer, EXCEPTION_BUFFER_SIZE - 1);
        }
    }

    /**
        Copy constructor.
    */
    Exception(const Exception&) = default;

protected:

    /**
        This protected method format and return 'C' string.
        Used in 'What' and 'Print' methods.
        Override this method and use protected 'text', 'file', 'line', 'func' members
        if you want to change 'Print' and 'What' information.
        @brief Format
        @return
    */
    virtual std::string_view Format() const
    {
        s_buffer[0] = '\0';

        if(std::strlen(m_text))
        {
            std::strncpy(s_buffer, m_text, EXCEPTION_BUFFER_SIZE - 2);
            std::strcat(s_buffer, "\n");
        }

        if(std::strlen(m_file))
        {
            std::strcat(s_buffer, "\tin file: ");
            std::strncat(s_buffer, m_file, EXCEPTION_BUFFER_SIZE - 2);
            std::strcat(s_buffer, "\n");
        }

        if(std::strlen(m_line))
        {
            std::strcat(s_buffer, "\tat line: ");
            std::strncat(s_buffer, m_line, EXCEPTION_BUFFER_SIZE - 2);
            std::strcat(s_buffer, "\n");
        }

        if(std::strlen(m_func))
        {
            std::strcat(s_buffer, "\tin function: ");
            std::strncat(s_buffer, m_func, EXCEPTION_BUFFER_SIZE - 2);
            std::strcat(s_buffer, "\n");
        }

        return std::string_view(s_buffer);
    }

public:
    /**
        This method return 'C' string that contain all information packed in exception object.
        @brief Information packed in exception object.
        @return Information packed in exception object.
    */
    const std::string_view What() const
    {
        return Format();
    }

    /**
        Print all information from exception object into stdout.
    */
    void Print() const
    {
        std::string_view str = Format();

        if(str.length())
            std::printf(str.begin());
    }

    /**
        Adds textual information to an exception object.
        @brief operator <<
        @param Information.
        @return Reference to the same object.
    */
    Exception& operator << (const char* str)
    {
        std::strncat(m_text, str, EXCEPTION_BUFFER_SIZE - std::strlen(m_text) - 1);
        return *this;        
    }

    /**
        Adds textual information to an exception object.
        @brief operator <<
        @param Information.
        @return Reference to the same object.
    */
    Exception& operator << (char ch)
    {
        char str[2];
        str[0] = ch;
        str[1] = '\0';

        std::strncat(m_text, str, EXCEPTION_BUFFER_SIZE - std::strlen(m_text) - 2 - 1);

        return *this;
    }

    /**
        Adds digital information to an exception object that can be accepted by the std::snprintf function.
        @brief operator <<
        @param Information.
        @return Reference to the same object.
    */
    template<typename T>
    Exception& operator << (T val)
    {
        static char str[EXCEPTION_BUFFER_SIZE];
        str[0] = '\0';
        std::snprintf(str, EXCEPTION_BUFFER_SIZE-1, "%d", val);

        std::strncat(m_text, str,
                     EXCEPTION_BUFFER_SIZE
                     - std::strlen(m_text) - std::strlen(str) - 1);

        return *this;
    }

    /**
        User defined information about exception.
        @return Copy of user defined information about exception.
    */
    std::string_view Text() const
    {
        return std::string_view(m_text);
    }

    /**
        File name of the source code where exception was caught.
        @return Copy of file name string.
    */
    std::string_view File() const
    {
        return std::string_view(m_file);
    }

    /**
        Line number in file of the source code where exception was caught.
        @return Copy of line number string.
    */
    std::string_view Line() const
    {
        return std::string_view(m_line);
    }

    /**
        Function name where exception was caught.
        @return Copy of file name string.
    */
    std::string_view Func() const
    {
        return std::string_view(m_func);
    }

protected:

    /**
        User defined information about exception.
        This member is protected and can be used in derived classes.
        @brief text
    */
    char m_text[EXCEPTION_BUFFER_SIZE] = {""};

    /**
        File name of the source code where exception was caught.
        This member is protected and can be used in derived classes.
        @brief file
    */
    char m_file[EXCEPTION_BUFFER_SIZE] = {"\tin file: "};

    /**
        Line number in file of the source code where exception was caught.
        This member is protected and can be used in derived classes.
        @brief line
    */
    char m_line[EXCEPTION_BUFFER_SIZE] = {"\tat line: "};

    /**
        Function name where exception was caught.
        This member is protected and can be used in derived classes.
        @brief func
    */
    char m_func[EXCEPTION_BUFFER_SIZE] = {"\tin function: "};

    static inline char s_buffer[EXCEPTION_BUFFER_SIZE * 5] = {""};
};//class Exception

/**
    This macros used for sending information into vgui::Eexception class objects.
    It sends name of current file, current line of code and name of function.
    Class have shift left operator and can recieve additional textual and digital information about exception.
    For example:
    void Foo()
    {
        int my_information = 123;
        ...
        throw ExceptionInfo << "One" << my_information << "Two" << "Three";
        ...
    }
*/

//#define ExceptionInfo Exception("", __FILE__, __LINE__, __func__)
//#define ExceptionInfo Exception("","",0,"")
