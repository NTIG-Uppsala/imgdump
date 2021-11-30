#pragma once
#include <drogon/HttpController.h>
using namespace drogon;
namespace api
{
class tset:public drogon::HttpController<tset>
{
  public:
    METHOD_LIST_BEGIN
    //use METHOD_ADD to add your custom processing function here;
    //METHOD_ADD(tset::get,"/{2}/{1}",Get);//path is /api/tset/{arg2}/{arg1}
    //METHOD_ADD(tset::your_method_name,"/{1}/{2}/list",Get);//path is /api/tset/{arg1}/{arg2}/list
    ADD_METHOD_TO(tset::poggers,"/upload_endpoint",Post);//path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(tset::ViewTestFileUpload,"/test/sendfile",Get);//path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:

    void poggers(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback);
    void ViewTestFileUpload(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback);

    // void your_method_name(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,double p1,int p2) const;
};
}
