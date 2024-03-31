#include "yqlua/QsSkin.h"
#include "QtCore/qtemporaryfile.h"
#include "yutil/file.h"
#include "yutil/json.h"
#include "yutil/codec.h"
#include "yqlua/QsWidget.h"
QsSkin::QsSkin()
{
    //std::cout << "[QsSkin] create " << std::hex << this << std::endl;
}

QsSkin::~QsSkin()
{
    //std::cout << "[QsSkin] destory " << std::hex << this << std::endl;
}

void QsSkin::regist(kaguya::State& T)
{
    T["QsSkin"].setClass(kaguya::UserdataMetatable<QsSkin>()
        .setConstructors<QsSkin()>()
        .addFunction("load", &QsSkin::load)
        .addFunction("widget", &QsSkin::widget)
        .addFunction("config", &QsSkin::config)
        .addFunction("pageSize", &QsSkin::pageSize)
        .addFunction("pageName", &QsSkin::pageName)
        .addFunction("existRes", &QsSkin::existRes)
        .addFunction("getResData", &QsSkin::getResData)
        .addFunction("getResFilename", &QsSkin::getResFilename)
    );
}

bool QsSkin::load(const std::string& filepath)
{
    ylib::buffer bufdata;
    if (!ylib::file::read(filepath, bufdata))
        return false;
    auto jsonData = ylib::json::from(codec::gzip::de(bufdata));

    for (size_t i = 0; i < jsonData["widgets"].size(); i++)
    {
        QsWidget* widget = new QsWidget();
        std::string name = jsonData["widgets"][i]["name"].to<std::string>();
        m_widgets[name] = widget;
        ylib::json*config = new ylib::json;
        *config = jsonData["widgets"][i]["config"];
        m_widgets_config[name] = config;
        m_widget_names.push_back(name);
    }
    m_init_json = jsonData["init"];
    // 解析RES
    {
        auto keys = m_init_json["res"]["datas"].keys();
        for (size_t i = 0; i < keys.size(); i++)
        {
            ResData *rd = new ResData;
            rd->data = codec::base64::de(m_init_json["res"]["datas"][keys[i]]["data"].to<std::string>());
            QTemporaryFile tf;
            tf.setAutoRemove(false);
            tf.open();
            tf.write(rd->data.data(),rd->data.length());
            tf.close();
            rd->filename = tf.fileName().toStdString();
            m_res.emplace(keys[i], rd);
        }
            
    }
    return true;
}
QsWidget* QsSkin::widget(const std::string& name)
{
    return m_widgets[name];
}

std::string QsSkin::config(const std::string& name)
{
    return m_widgets_config[name]->to_string();
}

int QsSkin::pageSize()
{
    return m_widgets.size();
}

std::string QsSkin::pageName(int index)
{
    return m_widget_names[index];
}
bool QsSkin::existRes(const std::string& name)
{
    return m_res.find(name) != m_res.end();
}
QsBuffer QsSkin::getResData(const std::string& name)
{
    QsBuffer result;
    auto iter = m_res.find(name);
    if (iter == m_res.end())
        return QsBuffer();
    (*(ylib::buffer*)&result) = iter->second.get()->data;
    return result;
}
std::string QsSkin::getResFilename(const std::string& name)
{
    auto iter = m_res.find(name);
    if (iter == m_res.end())
        return std::string();
    return iter->second.get()->filename;
}