#include "gldatashells.h"


// parent constructors
GlobShell::GlobShell() = default;
GlobShell::GlobShell(const string& name, GLuint ID)
   : name(name), ID(ID)
{}
// this template is used to find a GlobShell child in a vector by name attribute
template <class T>
      T* GlobShell::findByName(vector<T>& globs, const string &searchName, bool allowSearchFail) {
   // this error check is optimized out at compile time,
   // but ensures that type T is a child class of GlobShell
   (void)static_cast<GlobShell*>((T*)0); // TODO: error access through this->name
   // search through all members of the vector globs
   for(auto& glob : globs){
      if(glob.name == searchName){
         return &glob;
      }
   }
   // if we haven't returned yet there isn't a match in the vector
   if (allowSearchFail) {
      return nullptr;
   } else {
      cerr << "GlobShell::findByName() failed to find the name '" << searchName << "'" << endl;
      throw exception();
   }
}


// inherit constructors from parent
UniformShell::UniformShell() : GlobShell() {};
// for UniformShell, GLint instead of GLuint is passed to the constructor, so cast it
UniformShell::UniformShell(const string &name, GLint ID, GLenum type, void *location)
   : GlobShell(name, static_cast<GLuint>(ID)), type(type), location(location) {}
// use this wrapper to instantiate the template for UniformShell
UniformShell* UniformShell::findByName(vector<UniformShell> &globs, const string &searchName, bool allowSearchFail) {
   return GlobShell::findByName<UniformShell>(globs, searchName, allowSearchFail);
}


// inherit constructors from parent
TexShell::TexShell() : GlobShell() {};
TexShell::TexShell(const string &name, GLuint ID) : GlobShell(name, ID) {}
// use this wrapper to instantiate the findByName<> template for TexShell
TexShell *TexShell::findByName(vector<TexShell> &globs, const string &searchName, bool allowSearchFail) {
   return GlobShell::findByName<TexShell>(globs, searchName, allowSearchFail);
}


// inherit constructors from parent
ShaderShell::ShaderShell() : GlobShell() {};
// use this wrapper to instantiate the findByName<> template for ShaderShell
ShaderShell::ShaderShell(const string &name, GLuint ID) : GlobShell(name, ID) {}
ShaderShell *ShaderShell::findByName(vector<ShaderShell> &globs, const string &searchName, bool allowSearchFail) {
   return GlobShell::findByName<ShaderShell>(globs, searchName, allowSearchFail);
}


// inherit constructors from parent
MPTexShell::MPTexShell() : TexShell() {};
MPTexShell::MPTexShell(const string &name, GLuint ID) : TexShell(name, ID) {}
// use this wrapper to instantiate the findByName<> template for MPTexShell
MPTexShell *MPTexShell::findByName(vector<MPTexShell> &globs, const string &searchName, bool allowSearchFail) {
   return GlobShell::findByName<MPTexShell>(globs, searchName, allowSearchFail);
}
