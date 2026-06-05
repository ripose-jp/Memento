<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>AboutWindow</name>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="8"/>
        <source>About Memento</source>
        <translation>关于 Memento</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="29"/>
        <source>Memento</source>
        <translation>Memento</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="36"/>
        <source>Version %1</source>
        <translation>版本 %1</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="44"/>
        <source>GPLv2-Only</source>
        <translation>仅 GPLv2</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="50"/>
        <source>&lt;p&gt;&lt;a href=&quot;https://ripose-jp.github.io/Memento/&quot;&gt;Project Site&lt;/a&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;a href=&quot;https://ripose-jp.github.io/Memento/&quot;&gt;项目网站&lt;/a&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="57"/>
        <source>&lt;p&gt;&lt;a href=&quot;https://github.com/ripose-jp/Memento&quot;&gt;GitHub&lt;/a&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;a href=&quot;https://github.com/ripose-jp/Memento&quot;&gt;GitHub&lt;/a&gt;&lt;/p&gt;</translation>
    </message>
</context>
<context>
    <name>AnkiIntegrationHelpWindow</name>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="8"/>
        <source>Anki Integration Help</source>
        <translation>Anki 集成帮助</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="34"/>
        <source>Introduction</source>
        <translation>介绍</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="40"/>
        <source>This document describes the syntax and the associated markers available for use in note templates.</source>
        <translation>本文档介绍可在笔记模板中使用的语法和相关标记。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="47"/>
        <source>Syntax</source>
        <translation>语法</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="54"/>
        <source>&lt;p&gt;A marker is a section of text enclosed in curly braces like &lt;code&gt;{subtitle}&lt;/code&gt;. When Memento&apos;s template processor sees a marker, it replaces it with its associated value. In the case of &lt;code&gt;{subtitle}&lt;/code&gt; that is the currently displayed subtitle.&lt;/p&gt;&lt;p&gt;Some markers have arguments that allow their functionality to modified. For example, &lt;code&gt;{glossary : dict = 1}&lt;/code&gt; makes it so the glossary marker only uses entries from the dictionary with the ID of 1. The &lt;code&gt;:&lt;/code&gt; character indicates the beginning of marker arguments. Arguments are set in the form of &lt;code&gt;name = value&lt;/code&gt;. If a marker has multiple arguments, arguments are separated by the &lt;code&gt;,&lt;/code&gt; character. An example of a marker with multiple arguments is &lt;code&gt;{screenshot : max-width = 1280, max-height = 720}&lt;/code&gt;.&lt;/p&gt;&lt;p&gt;A single marker can specify whether to use one or another value. For example, &lt;code&gt;{selection | clipboard}&lt;/code&gt; specifies if &lt;code&gt;selection&lt;/code&gt; is empty, use &lt;code&gt;clipboard&lt;/code&gt; instead. This functionality is triggered by the &lt;code&gt;|&lt;/code&gt; character. Multiple ors can be chained together like &lt;code&gt;{selection | clipboard | glossary : dict = 1}&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;标记是一段用花括号括起来的文本，例如 &lt;code&gt;{subtitle}&lt;/code&gt;。当 Memento 的模板处理器看到标记时，会将其替换为对应的值。对于 &lt;code&gt;{subtitle}&lt;/code&gt;，该值就是当前显示的字幕。&lt;/p&gt;&lt;p&gt;某些标记带有可修改其功能的参数。例如，&lt;code&gt;{glossary : dict = 1}&lt;/code&gt; 会让释义标记只使用 ID 为 1 的词典条目。&lt;code&gt;:&lt;/code&gt; 字符表示标记参数的开始。参数以 &lt;code&gt;name = value&lt;/code&gt; 的形式设置。如果一个标记有多个参数，参数之间用 &lt;code&gt;,&lt;/code&gt; 字符分隔。带有多个参数的标记示例是 &lt;code&gt;{screenshot : max-width = 1280, max-height = 720}&lt;/code&gt;。&lt;/p&gt;&lt;p&gt;单个标记可以指定在某个值为空时使用另一个值。例如，&lt;code&gt;{selection | clipboard}&lt;/code&gt; 指定当 &lt;code&gt;selection&lt;/code&gt; 为空时改用 &lt;code&gt;clipboard&lt;/code&gt;。此功能由 &lt;code&gt;|&lt;/code&gt; 字符触发。可以像 &lt;code&gt;{selection | clipboard | glossary : dict = 1}&lt;/code&gt; 这样串联多个备选项。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="82"/>
        <source>Term Markers</source>
        <translation>词条标记</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="88"/>
        <source>This section describes the markers available for use in term notes.</source>
        <translation>本节介绍可在词条笔记中使用的标记。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="111"/>
        <source>Kanji Markers</source>
        <translation>汉字标记</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="117"/>
        <source>This section describes the markers available for use in kanji notes.</source>
        <translation>本节介绍可在汉字笔记中使用的标记。</translation>
    </message>
</context>
<context>
    <name>AnkiIntegrationPage</name>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="89"/>
        <source>Add Profile</source>
        <translation>添加配置</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="95"/>
        <source>Profile name</source>
        <translation>配置名称</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="101"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="106"/>
        <source>Copy current profile</source>
        <translation>复制当前配置</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="206"/>
        <source>General</source>
        <translation>常规</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="216"/>
        <source>Enable Anki integration</source>
        <translation>启用 Anki 集成</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="233"/>
        <source>Show advanced settings</source>
        <translation>显示高级设置</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="247"/>
        <source>Profile</source>
        <translation>配置</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="257"/>
        <source>Current profile</source>
        <translation>当前配置</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="304"/>
        <source>A profile with this name already exists</source>
        <translation>已存在同名配置</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="313"/>
        <source>AnkiConnect</source>
        <translation>AnkiConnect</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="323"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="328"/>
        <source>Hostname</source>
        <translation>主机名</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="353"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="357"/>
        <source>Port</source>
        <translation>端口</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="375"/>
        <source>Use API key</source>
        <translation>使用 API 密钥</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="395"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="399"/>
        <source>API key</source>
        <translation>API 密钥</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="413"/>
        <source>Test connection</source>
        <translation>测试连接</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="417"/>
        <source>Connect</source>
        <translation>连接</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="424"/>
        <source>Anki Connection</source>
        <translation>Anki 连接</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="425"/>
        <source>Connection successful.</source>
        <translation>连接成功。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="430"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="714"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="751"/>
        <source>Anki Connection Error</source>
        <translation>Anki 连接错误</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="431"/>
        <source>Failed to connect to Anki: %1.</source>
        <translation>无法连接到 Anki：%1。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="447"/>
        <source>Advanced</source>
        <translation>高级</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="457"/>
        <source>Duplicate policy</source>
        <translation>重复处理策略</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="464"/>
        <source>None</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="468"/>
        <source>Allowed in different decks</source>
        <translation>允许在不同牌组中重复</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="472"/>
        <source>Allowed in same deck</source>
        <translation>允许在同一牌组中重复</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="491"/>
        <source>Replace newlines with</source>
        <translation>将换行替换为</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="495"/>
        <source>Replacement string</source>
        <translation>替换字符串</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="509"/>
        <source>Screenshot file type</source>
        <translation>截图文件类型</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="516"/>
        <source>JPEG</source>
        <translation>JPEG</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="520"/>
        <source>PNG</source>
        <translation>PNG</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="524"/>
        <source>WebP</source>
        <translation>WebP</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="543"/>
        <source>Audio padding start duration</source>
        <translation>音频开始填充时长</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="548"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="576"/>
        <source>Seconds</source>
        <translation>秒</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="571"/>
        <source>Audio padding end duration</source>
        <translation>音频结束填充时长</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="599"/>
        <source>Audio media normalization</source>
        <translation>音频媒体标准化</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="616"/>
        <source>Audio normalization dB</source>
        <translation>音频标准化 dB</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="621"/>
        <source>dB</source>
        <translation>dB</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="642"/>
        <source>Include in glossary</source>
        <translation>包含在释义中</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="669"/>
        <source>Cards</source>
        <translation>卡片</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="679"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="684"/>
        <source>Tags</source>
        <translation>标签</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="696"/>
        <source>Term Notes</source>
        <translation>词条笔记</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="715"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="752"/>
        <source>Could not get model fields: %1.</source>
        <translation>无法获取模型字段：%1。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="733"/>
        <source>Kanji Notes</source>
        <translation>汉字笔记</translation>
    </message>
</context>
<context>
    <name>AnkiMarkerDescription</name>
    <message>
        <location filename="../qml/controls/AnkiMarkerDescription.qml" line="52"/>
        <source>&lt;p&gt;&lt;b&gt;Default Value:&lt;/b&gt; %1&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;默认值：&lt;/b&gt;%1&lt;/p&gt;</translation>
    </message>
</context>
<context>
    <name>AnkiMarkers</name>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="48"/>
        <source>&lt;p&gt;Audio of the subtitle list selection.&lt;/p&gt;</source>
        <translation>&lt;p&gt;字幕列表所选内容的音频。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="51"/>
        <source>&lt;p&gt;Audio of the word if found and the skip hash is not matched.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果找到单词且跳过哈希不匹配，则为该单词的音频。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="54"/>
        <source>&lt;p&gt;Audio of the current subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;当前字幕的音频。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="57"/>
        <source>&lt;p&gt;The kanji character.&lt;/p&gt;</source>
        <translation>&lt;p&gt;汉字字符。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="60"/>
        <source>&lt;p&gt;The current text in the clipboard.&lt;/p&gt;</source>
        <translation>&lt;p&gt;剪贴板中的当前文本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="63"/>
        <source>&lt;p&gt;The raw expression as recognized by Memento.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Memento 识别到的原始表达。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="66"/>
        <source>&lt;p&gt;The text before &lt;code&gt;{%1}&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;code&gt;{%1}&lt;/code&gt; 之前的文本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="69"/>
        <source>&lt;p&gt;The text after &lt;code&gt;{%1}&lt;/code&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;code&gt;{%1}&lt;/code&gt; 之后的文本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="72"/>
        <source>&lt;p&gt;Text of all selected lines in the secondary subtitle list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;次要字幕列表中所有选中行的文本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="75"/>
        <source>&lt;p&gt;Text of all selected lines in the primary subtitle list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;主要字幕列表中所有选中行的文本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="78"/>
        <source>&lt;p&gt;The expression in kanji. Kana if there is no kanji or explicitly requested.&lt;/p&gt;</source>
        <translation>&lt;p&gt;汉字形式的表达。没有汉字或明确请求时为假名。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="81"/>
        <source>&lt;p&gt;The average of frequency data for the expression. Defaults to 0 occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;该表达频率数据的平均值。没有频率数据时出现次数默认为 0。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="85"/>
        <source>&lt;p&gt;The average of frequency data for the expression. Defaults to 9999999 when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;该表达频率数据的平均值。没有频率数据时默认为 9999999。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="89"/>
        <source>&lt;p&gt;The harmonic mean of frequency data for the expression. Defaults to 0 for occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;该表达频率数据的调和平均值。没有频率数据时出现次数默认为 0。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="93"/>
        <source>&lt;p&gt;The harmonic mean of frequency data for the expression. Defaults to 9999999 for occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;该表达频率数据的调和平均值。没有频率数据时出现次数默认为 9999999。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="97"/>
        <source>&lt;p&gt;Bulleted list of frequency tags.&lt;/p&gt;</source>
        <translation>&lt;p&gt;频率标签的项目符号列表。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="100"/>
        <source>&lt;p&gt;The expression in kanji with furigana above. Kana if there is no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;带上方振假名的汉字表达。没有汉字时为假名。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="104"/>
        <source>&lt;p&gt;The expression in kanji with furigana proceeding in brackets. Kana if there is no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;汉字表达，后接括号中的振假名。没有汉字时为假名。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="108"/>
        <source>&lt;p&gt;Dictionary definitions of the expression without dictionary information.&lt;/p&gt;</source>
        <translation>&lt;p&gt;不含词典信息的表达释义。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="111"/>
        <source>&lt;p&gt;Dictionary definitions of the expression in compact form.&lt;/p&gt;</source>
        <translation>&lt;p&gt;紧凑形式的表达释义。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="114"/>
        <source>&lt;p&gt;Dictionary definitions of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;表达的词典释义。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="117"/>
        <source>&lt;p&gt;A comma separated list of kunyomi (Japanese) readings written in katakana.&lt;/p&gt;</source>
        <translation>&lt;p&gt;以逗号分隔的训读（日语读法）列表，使用片假名书写。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="120"/>
        <source>&lt;p&gt;A comma separated list of onyomi (Chinese) readings written in hiragana.&lt;/p&gt;</source>
        <translation>&lt;p&gt;以逗号分隔的音读（汉语读法）列表，使用平假名书写。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="123"/>
        <source>&lt;p&gt;A comma separated list of pitch accent categories for the expression: heiban, kifuku, atamadaka, nakadaka, odaka.&lt;/p&gt;</source>
        <translation>&lt;p&gt;该表达的音高重音类别逗号分隔列表：heiban、kifuku、atamadaka、nakadaka、odaka。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="127"/>
        <source>&lt;p&gt;Pitch accents for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;表达读音的音高重音。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="130"/>
        <source>&lt;p&gt;Pitch graphs for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;表达读音的音高图。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="133"/>
        <source>&lt;p&gt;Pitch positions for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;表达读音的音高位置。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="136"/>
        <source>&lt;p&gt;The reading of the expression in kana. Empty if the expression has no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;以假名表示的表达读音。表达没有汉字时为空。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="140"/>
        <source>&lt;p&gt;Screenshot of the current frame.&lt;/p&gt;</source>
        <translation>&lt;p&gt;当前帧的截图。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="143"/>
        <source>&lt;p&gt;Screenshot of the current frame without subtitles if visible.&lt;/p&gt;</source>
        <translation>&lt;p&gt;当前帧的截图；如果字幕可见，则不包含字幕。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="146"/>
        <source>&lt;p&gt;The currently selected glossary text. If multiple glossaries are selected, they are ordered as a list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;当前选中的释义文本。如果选中多个释义，它们会按列表排序。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="150"/>
        <source>&lt;p&gt;The number of strokes in the character.&lt;/p&gt;</source>
        <translation>&lt;p&gt;字符的笔画数。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="153"/>
        <source>&lt;p&gt;The text of the secondary subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;次要字幕的文本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="156"/>
        <source>&lt;p&gt;The text of the primary subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;主要字幕的文本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="159"/>
        <source>&lt;p&gt;Unordered list of expression tags without descriptions.&lt;/p&gt;</source>
        <translation>&lt;p&gt;不含说明的表达标签无序列表。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="162"/>
        <source>&lt;p&gt;Unordered list of expression tags.&lt;/p&gt;</source>
        <translation>&lt;p&gt;表达标签的无序列表。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="165"/>
        <source>&lt;p&gt;Title of the current media. Filename if no title is set.&lt;/p&gt;</source>
        <translation>&lt;p&gt;当前媒体的标题。未设置标题时为文件名。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="168"/>
        <source>&lt;p&gt;An MP4 file containing a clip of the current context.&lt;/p&gt;</source>
        <translation>&lt;p&gt;包含当前上下文片段的 MP4 文件。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="184"/>
        <source>&lt;p&gt;If true, audio is included in the clip.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果为 true，则片段包含音频。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="187"/>
        <source>&lt;p&gt;The decibel value to normalize audio to. Ignored if &lt;code&gt;%1 = false&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;音频标准化目标分贝值。如果 &lt;code&gt;%1 = false&lt;/code&gt; 则忽略。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="191"/>
        <source>&lt;p&gt;Filter to only use entries from a particular dictionary. Takes a dictionary ID. Dictionary IDs can be found in &lt;b&gt;Options&lt;/b&gt; under &lt;b&gt;Dictionaries&lt;/b&gt; at the end of dictionary names.&lt;/p&gt;</source>
        <translation>&lt;p&gt;筛选为只使用特定词典中的条目。接受词典 ID。词典 ID 可在&lt;b&gt;选项&lt;/b&gt;的&lt;b&gt;词典&lt;/b&gt;中，在词典名称末尾找到。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="196"/>
        <source>&lt;p&gt;If true, maintains the aspect ratio of the image during resize.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果为 true，调整大小时保持图像宽高比。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="199"/>
        <source>&lt;p&gt;Used to determine the maximum height of a saved screenshot. A value of -1 leaves this unbounded.&lt;/p&gt;</source>
        <translation>&lt;p&gt;用于确定保存截图的最大高度。值为 -1 表示不限制。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="203"/>
        <source>&lt;p&gt;Used to determine the maximum width of a saved screenshot. A value of -1 leaves this unbounded.&lt;/p&gt;</source>
        <translation>&lt;p&gt;用于确定保存截图的最大宽度。值为 -1 表示不限制。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="207"/>
        <source>&lt;p&gt;If true, only export the lowest frequency value.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果为 true，只导出最低频率值。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="210"/>
        <source>&lt;p&gt;If true, normalize audio.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果为 true，则标准化音频。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="213"/>
        <source>&lt;p&gt;The source of clip timings. Possible values are:&lt;ul&gt;&lt;li&gt;&lt;b&gt;subtitle&lt;/b&gt;: Use timings from the current subtitle.&lt;/li&gt;&lt;li&gt;&lt;b&gt;context&lt;/b&gt;: Use timings from the currently selected context.&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;片段时间的来源。可用值为：&lt;ul&gt;&lt;li&gt;&lt;b&gt;subtitle&lt;/b&gt;：使用当前字幕的时间。&lt;/li&gt;&lt;li&gt;&lt;b&gt;context&lt;/b&gt;：使用当前选中上下文的时间。&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="222"/>
        <source>&lt;p&gt;If true, subtitles are included in the clip.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果为 true，则片段包含字幕。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="225"/>
        <source>&lt;p&gt;If true, export frequency values without dictionary names.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果为 true，导出不含词典名称的频率值。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="229"/>
        <location filename="../qml/util/AnkiMarkers.qml" line="232"/>
        <location filename="../qml/util/AnkiMarkers.qml" line="238"/>
        <source>&lt;code&gt;true&lt;/code&gt;</source>
        <translation>&lt;code&gt;true&lt;/code&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="230"/>
        <source>The profile normalization dB setting</source>
        <translation>配置的标准化 dB 设置</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="231"/>
        <source>None</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="233"/>
        <location filename="../qml/util/AnkiMarkers.qml" line="234"/>
        <source>&lt;code&gt;-1&lt;/code&gt;</source>
        <translation>&lt;code&gt;-1&lt;/code&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="235"/>
        <location filename="../qml/util/AnkiMarkers.qml" line="239"/>
        <source>&lt;code&gt;false&lt;/code&gt;</source>
        <translation>&lt;code&gt;false&lt;/code&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="236"/>
        <source>The profile audio media normalization setting</source>
        <translation>配置的音频媒体标准化设置</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="237"/>
        <source>&lt;code&gt;subtitle&lt;/code&gt;</source>
        <translation>&lt;code&gt;subtitle&lt;/code&gt;</translation>
    </message>
</context>
<context>
    <name>AnkiNoteBox</name>
    <message>
        <location filename="../qml/controls/AnkiNoteBox.qml" line="34"/>
        <source>Deck</source>
        <translation>牌组</translation>
    </message>
    <message>
        <location filename="../qml/controls/AnkiNoteBox.qml" line="52"/>
        <source>Model</source>
        <translation>模型</translation>
    </message>
</context>
<context>
    <name>ApplicationPage</name>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="66"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
    <message>
        <source>Interface language</source>
        <translation type="vanished">界面语言</translation>
    </message>
    <message>
        <source>Interface language (requires restart)</source>
        <translation type="vanished">界面语言（需要重启）</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="76"/>
        <source>Interface language (Restart required)</source>
        <translation>界面语言（需要重启）</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="83"/>
        <source>System</source>
        <translation>系统</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="118"/>
        <source>Update</source>
        <translation>更新</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="128"/>
        <source>Check for updates on launch</source>
        <translation>启动时检查更新</translation>
    </message>
</context>
<context>
    <name>AudioSourcesPage</name>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="65"/>
        <source>Audio Sources Help</source>
        <translation>音频源帮助</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="70"/>
        <source>&lt;p&gt;&lt;b&gt;Source Name&lt;/b&gt;: The name of the audio source as it will appear in Memento.&lt;/p&gt;
                        &lt;p&gt;&lt;b&gt;URL&lt;/b&gt;: The URL of the audio source. Supports inserting &lt;b&gt;{expression}&lt;/b&gt; and
                            &lt;b&gt;{reading}&lt;/b&gt; markers into the URL.&lt;/p&gt;
                        &lt;p&gt;&lt;b&gt;MD5 Skip Hash&lt;/b&gt;: Audio that matches this MD5 hash will be ignored.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;源名称&lt;/b&gt;：音频源在 Memento 中显示的名称。&lt;/p&gt;&lt;p&gt;&lt;b&gt;URL&lt;/b&gt;：音频源的 URL。支持在 URL 中插入 &lt;b&gt;{expression}&lt;/b&gt; 和 &lt;b&gt;{reading}&lt;/b&gt; 标记。&lt;/p&gt;&lt;p&gt;&lt;b&gt;MD5 跳过哈希&lt;/b&gt;：与此 MD5 哈希匹配的音频会被忽略。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="112"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="119"/>
        <source>URL</source>
        <translation>URL</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="128"/>
        <source>File</source>
        <translation>文件</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="132"/>
        <source>JSON</source>
        <translation>JSON</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="143"/>
        <source>MD5 Skip Hash</source>
        <translation>MD5 跳过哈希</translation>
    </message>
</context>
<context>
    <name>BehaviorPage</name>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="15"/>
        <source>Select Custom Open Folder</source>
        <translation>选择自定义打开文件夹</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="72"/>
        <source>Player</source>
        <translation>播放器</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="82"/>
        <source>Auto fit media on load</source>
        <translation>加载媒体时自动调整窗口大小</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="99"/>
        <source>Auto fit media percentage</source>
        <translation>窗口自动调整比例</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="117"/>
        <source>On Screen Controller</source>
        <translation>屏幕控制器</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="127"/>
        <source>Visibility duration milliseconds</source>
        <translation>可见持续时间（毫秒）</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="147"/>
        <source>Fade duration milliseconds</source>
        <translation>淡出持续时间（毫秒）</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="167"/>
        <source>Minimum mouse pixel movement</source>
        <translation>最小鼠标移动像素</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="187"/>
        <source>Allow mpv to control cursor visibility</source>
        <translation>允许 mpv 控制光标可见性</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="204"/>
        <source>Show preview thumbnails</source>
        <translation>显示预览缩略图</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="219"/>
        <source>Subtitles</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="229"/>
        <source>Automatically pause at the end of subtitles</source>
        <translation>在字幕结尾自动暂停</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="246"/>
        <source>Show subtitles when the cursor is at the bottom of the player</source>
        <translation>光标在播放器底部时显示字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="263"/>
        <source>Show secondary subtitles when the cursor is at the top of the player</source>
        <translation>光标在播放器顶部时显示次要字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="279"/>
        <source>Open File Directory</source>
        <translation>打开文件目录</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="289"/>
        <source>Open file starting location</source>
        <translation>打开文件起始位置</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="296"/>
        <source>Current</source>
        <translation>当前</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="300"/>
        <source>Custom</source>
        <translation>自定义</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="304"/>
        <source>Documents</source>
        <translation>文档</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="308"/>
        <source>Home</source>
        <translation>主文件夹</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="312"/>
        <source>Movies</source>
        <translation>影片</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="331"/>
        <source>Custom file directory</source>
        <translation>自定义文件目录</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="335"/>
        <source>Browse</source>
        <translation>浏览</translation>
    </message>
</context>
<context>
    <name>DatabaseManager</name>
    <message>
        <location filename="../dict/databasemanager.cpp" line="395"/>
        <location filename="../dict/databasemanager.cpp" line="526"/>
        <source>Could not prepare database query</source>
        <translation>无法准备数据库查询</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="404"/>
        <location filename="../dict/databasemanager.cpp" line="416"/>
        <location filename="../dict/databasemanager.cpp" line="428"/>
        <location filename="../dict/databasemanager.cpp" line="534"/>
        <source>Could not bind values to statement</source>
        <translation>无法将值绑定到语句</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="463"/>
        <source>Error when executing sqlite query. Code %1</source>
        <translation>执行 SQLite 查询时出错。代码 %1</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="473"/>
        <source>Error getting term information</source>
        <translation>获取词条信息时出错</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="608"/>
        <source>Error while executing kanji query</source>
        <translation>执行汉字查询时出错</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1105"/>
        <source>Could not add index.json</source>
        <translation>无法添加 index.json</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1107"/>
        <source>Could not add kanji terms</source>
        <translation>无法添加汉字词条</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1109"/>
        <source>Could not add kanji metadata</source>
        <translation>无法添加汉字元数据</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1111"/>
        <source>Could not add tags</source>
        <translation>无法添加标签</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1113"/>
        <source>Could not add terms</source>
        <translation>无法添加词条</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1115"/>
        <source>Could not add term metadata</source>
        <translation>无法添加词条元数据</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1117"/>
        <source>Could not open database</source>
        <translation>无法打开数据库</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1119"/>
        <source>Database is of a newer version</source>
        <translation>数据库版本较新</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1121"/>
        <source>Could not open dictionary file</source>
        <translation>无法打开词典文件</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1123"/>
        <source>Could not execute delete query on database</source>
        <translation>无法在数据库上执行删除查询</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1125"/>
        <source>Could not extract dictionary resources</source>
        <translation>无法提取词典资源</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1127"/>
        <source>Could not remove dictionary resources</source>
        <translation>无法移除词典资源</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1129"/>
        <source>Unknown error</source>
        <translation>未知错误</translation>
    </message>
</context>
<context>
    <name>Deconjugator</name>
    <message>
        <location filename="../dict/deconjugator.cpp" line="475"/>
        <source>godan verb</source>
        <translation>五段动词</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="477"/>
        <source>ichidan verb</source>
        <translation>一段动词</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="479"/>
        <source>suru verb</source>
        <translation>する 动词</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="481"/>
        <source>kuru verb</source>
        <translation>くる 动词</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="483"/>
        <source>adjective</source>
        <translation>形容词</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="485"/>
        <source>negative</source>
        <translation>否定</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="487"/>
        <source>past</source>
        <translation>过去</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="489"/>
        <source>-te</source>
        <translation>て形</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="491"/>
        <source>masu stem</source>
        <translation>ます词干</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="493"/>
        <source>volitional</source>
        <translation>意志形</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="495"/>
        <source>passive</source>
        <translation>被动</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="497"/>
        <source>causative</source>
        <translation>使役</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="499"/>
        <source>imperative</source>
        <translation>命令</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="501"/>
        <source>potential</source>
        <translation>可能</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="503"/>
        <source>potential or passive</source>
        <translation>可能或被动</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="505"/>
        <source>imperative negative</source>
        <translation>否定命令</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="507"/>
        <source>-zaru</source>
        <translation>-ざる</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="509"/>
        <source>-neba</source>
        <translation>-ねば</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="511"/>
        <source>-zu</source>
        <translation>-ず</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="513"/>
        <source>-nu</source>
        <translation>-ぬ</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="515"/>
        <source>-ba</source>
        <translation>-ば</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="517"/>
        <source>-tari</source>
        <translation>-たり</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="519"/>
        <source>-shimau</source>
        <translation>-しまう</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="521"/>
        <source>-chau</source>
        <translation>-ちゃう</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="523"/>
        <source>-chimau</source>
        <translation>-ちまう</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="525"/>
        <source>polite</source>
        <translation>礼貌</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="527"/>
        <source>-tara</source>
        <translation>-たら</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="529"/>
        <source>-tai</source>
        <translation>-たい</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="531"/>
        <source>-nasai</source>
        <translation>-なさい</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="533"/>
        <source>-sugiru</source>
        <translation>-すぎる</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="535"/>
        <source>-sou</source>
        <translation>-そう</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="537"/>
        <source>-e</source>
        <translation>-え</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="539"/>
        <source>-ki</source>
        <translation>-き</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="541"/>
        <source>-toku</source>
        <translation>-とく</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="543"/>
        <source>colloquial negative</source>
        <translation>口语否定</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="545"/>
        <source>provisional colloquial negative</source>
        <translation>条件口语否定</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="547"/>
        <source>progressive or perfect</source>
        <translation>进行或完成</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="549"/>
        <source>adv</source>
        <translation>副词</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="551"/>
        <source>noun</source>
        <translation>名词</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="553"/>
        <source>unknown</source>
        <translation>未知</translation>
    </message>
</context>
<context>
    <name>DictionaryPage</name>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="17"/>
        <source>Dictionary Files (*.zip)</source>
        <translation>词典文件 (*.zip)</translation>
    </message>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="18"/>
        <source>Select Dictionaries</source>
        <translation>选择词典</translation>
    </message>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="33"/>
        <source>Dictionary Error</source>
        <translation>词典错误</translation>
    </message>
</context>
<context>
    <name>InterfacePage</name>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="66"/>
        <source>Theme</source>
        <translation>主题</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="76"/>
        <source>Use system icons</source>
        <translation>使用系统图标</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="92"/>
        <source>Subtitle</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="102"/>
        <source>Font</source>
        <translation>字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="111"/>
        <source>Select Subtitle Font</source>
        <translation>选择字幕字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="125"/>
        <location filename="../qml/options/InterfacePage.qml" line="403"/>
        <location filename="../qml/options/InterfacePage.qml" line="468"/>
        <location filename="../qml/options/InterfacePage.qml" line="533"/>
        <location filename="../qml/options/InterfacePage.qml" line="598"/>
        <location filename="../qml/options/InterfacePage.qml" line="663"/>
        <location filename="../qml/options/InterfacePage.qml" line="728"/>
        <location filename="../qml/options/InterfacePage.qml" line="883"/>
        <location filename="../qml/options/InterfacePage.qml" line="948"/>
        <source>Bold</source>
        <translation>粗体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="146"/>
        <location filename="../qml/options/InterfacePage.qml" line="424"/>
        <location filename="../qml/options/InterfacePage.qml" line="489"/>
        <location filename="../qml/options/InterfacePage.qml" line="554"/>
        <location filename="../qml/options/InterfacePage.qml" line="619"/>
        <location filename="../qml/options/InterfacePage.qml" line="684"/>
        <location filename="../qml/options/InterfacePage.qml" line="749"/>
        <location filename="../qml/options/InterfacePage.qml" line="904"/>
        <location filename="../qml/options/InterfacePage.qml" line="969"/>
        <source>Italics</source>
        <translation>斜体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="167"/>
        <source>Line spacing</source>
        <translation>行距</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="173"/>
        <location filename="../qml/options/InterfacePage.qml" line="242"/>
        <source>Pixels</source>
        <translation>像素</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="187"/>
        <source>Line height as a percentage of window height</source>
        <translation>行高占窗口高度的百分比</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="196"/>
        <location filename="../qml/options/InterfacePage.qml" line="219"/>
        <source>Percent</source>
        <translation>百分比</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="210"/>
        <source>Bottom offset as a percentage of window height</source>
        <translation>底部偏移占窗口高度的百分比</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="233"/>
        <source>Stroke size</source>
        <translation>描边大小</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="256"/>
        <location filename="../qml/options/InterfacePage.qml" line="1018"/>
        <source>Text color</source>
        <translation>文本颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="269"/>
        <source>Select Subtitle Text Color</source>
        <translation>选择字幕文本颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="284"/>
        <location filename="../qml/options/InterfacePage.qml" line="990"/>
        <source>Background color</source>
        <translation>背景颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="297"/>
        <source>Select Subtitle Background Color</source>
        <translation>选择字幕背景颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="312"/>
        <source>Stroke color</source>
        <translation>描边颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="325"/>
        <source>Select Subtitle Stroke Color</source>
        <translation>选择字幕描边颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="370"/>
        <source>Search</source>
        <translation>搜索</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="380"/>
        <source>Term Expression Font</source>
        <translation>词条表记字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="389"/>
        <source>Select Term Expression Font</source>
        <translation>选择词条表记字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="445"/>
        <source>Term Reading Font</source>
        <translation>词条读音字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="454"/>
        <source>Select Term Reading Font</source>
        <translation>选择词条读音字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="510"/>
        <source>Term Conjugation Explanation Font</source>
        <translation>词条活用说明字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="519"/>
        <source>Select Term Conjugation Explanation Font</source>
        <translation>选择词条活用说明字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="575"/>
        <source>Tag Font</source>
        <translation>标签字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="584"/>
        <source>Select Tag Font</source>
        <translation>选择标签字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="640"/>
        <source>Glossary Font</source>
        <translation>释义字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="649"/>
        <source>Select Glossary Font</source>
        <translation>选择释义字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="705"/>
        <source>Kanji Font</source>
        <translation>汉字字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="714"/>
        <source>Select Kanji Font</source>
        <translation>选择汉字字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="768"/>
        <source>Popup</source>
        <translation>弹出窗口</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="778"/>
        <source>Popup width</source>
        <translation>弹出窗口宽度</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="798"/>
        <source>Popup height</source>
        <translation>弹出窗口高度</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="816"/>
        <source>Subtitle List</source>
        <translation>字幕列表</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="826"/>
        <location filename="../qml/options/InterfacePage.qml" line="1139"/>
        <source>Open in a separate window</source>
        <translation>在单独窗口中打开</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="843"/>
        <source>Show timestamps</source>
        <translation>显示时间戳</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="860"/>
        <source>Primary subtitle font</source>
        <translation>主要字幕字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="869"/>
        <source>Select Subtitle List Primary Font</source>
        <translation>选择字幕列表主要字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="925"/>
        <source>Secondary subtitle font</source>
        <translation>次要字幕字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="934"/>
        <source>Select Subtitle List Secondary Font</source>
        <translation>选择字幕列表次要字体</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1003"/>
        <source>Select Subtitle List Background Color</source>
        <translation>选择字幕列表背景颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1031"/>
        <source>Select Subtitle List Text Color</source>
        <translation>选择字幕列表文本颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1046"/>
        <source>Hover color</source>
        <translation>悬停颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1059"/>
        <source>Select Subtitle List Hover Color</source>
        <translation>选择字幕列表悬停颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1074"/>
        <source>Selection color</source>
        <translation>选中颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1087"/>
        <source>Select Subtitle List Selection Color</source>
        <translation>选择字幕列表选中颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1102"/>
        <source>Hovered selection color</source>
        <translation>悬停选中颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1115"/>
        <source>Select Subtitle List Hovered Selection Color</source>
        <translation>选择字幕列表悬停选中颜色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1129"/>
        <source>Auxiliary Search</source>
        <translation>辅助搜索</translation>
    </message>
</context>
<context>
    <name>KanjiEntry</name>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="55"/>
        <source>Error Adding Anki Note</source>
        <translation>添加 Anki 笔记时出错</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="151"/>
        <source>Could Not Open Anki</source>
        <translation>无法打开 Anki</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="230"/>
        <source>Glossary</source>
        <translation>释义</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="237"/>
        <source>Reading</source>
        <translation>读音</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="244"/>
        <source>Statistics</source>
        <translation>统计</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="312"/>
        <source>Classifications</source>
        <translation>分类</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="341"/>
        <source>Codepoints</source>
        <translation>码位</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="370"/>
        <source>Dictionary Indices</source>
        <translation>词典索引</translation>
    </message>
</context>
<context>
    <name>KeybindCommand</name>
    <message>
        <location filename="../qml/controls/KeybindCommand.qml" line="75"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../qml/controls/KeybindCommand.qml" line="79"/>
        <source>Unbound</source>
        <translation>未绑定</translation>
    </message>
    <message>
        <location filename="../qml/controls/KeybindCommand.qml" line="93"/>
        <source>Clear</source>
        <translation>清除</translation>
    </message>
</context>
<context>
    <name>KeybindsPage</name>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="53"/>
        <source>Add Profile</source>
        <translation>添加配置</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="59"/>
        <source>Profile name</source>
        <translation>配置名称</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="65"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="70"/>
        <source>Copy current profile</source>
        <translation>复制当前配置</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="178"/>
        <source>Add current expression to Anki</source>
        <translation>将当前表达添加到 Anki</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="193"/>
        <source>Scroll to previous search result</source>
        <translation>滚动到上一个搜索结果</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="208"/>
        <source>Scroll to next search result</source>
        <translation>滚动到下一个搜索结果</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="223"/>
        <source>Open file</source>
        <translation>打开文件</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="238"/>
        <source>Open URL</source>
        <translation>打开 URL</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="253"/>
        <source>Cycle OSC visibility</source>
        <translation>循环切换 OSC 可见性</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="268"/>
        <source>Show auxiliary search</source>
        <translation>显示辅助搜索</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="283"/>
        <source>Show subtitle list</source>
        <translation>显示字幕列表</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="300"/>
        <source>Start OCR</source>
        <translation>开始 OCR</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="315"/>
        <source>Toggle subtitle auto pause</source>
        <translation>切换字幕自动暂停</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="330"/>
        <source>Toggle subtitle visibility</source>
        <translation>切换字幕可见性</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="345"/>
        <source>Decrease subtitle size</source>
        <translation>减小字幕大小</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="360"/>
        <source>Increase subtitle size</source>
        <translation>增大字幕大小</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="375"/>
        <source>Move subtitle up</source>
        <translation>上移字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="390"/>
        <source>Move subtitle down</source>
        <translation>下移字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="405"/>
        <source>Copy subtitle list selection</source>
        <translation>复制字幕列表所选内容</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="420"/>
        <source>Subtitle list find</source>
        <translation>字幕列表查找</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="435"/>
        <source>Subtitle list find next</source>
        <translation>字幕列表查找下一个</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="450"/>
        <source>Subtitle list find previous</source>
        <translation>字幕列表查找上一个</translation>
    </message>
</context>
<context>
    <name>Main</name>
    <message>
        <location filename="../qml/Main.qml" line="14"/>
        <source>%1 - Memento</source>
        <translation>%1 - Memento</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="14"/>
        <source>Memento</source>
        <translation>Memento</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="263"/>
        <source>Search</source>
        <translation>搜索</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="287"/>
        <source>Subtitle List</source>
        <translation>字幕列表</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="327"/>
        <source>No Dictionaries Installed</source>
        <translation>未安装词典</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="333"/>
        <source>&lt;p&gt;For searching to work, please install a dictionary.&lt;/p&gt;&lt;p&gt;Dictionaries can be found &lt;a href=&apos;https://yomitan.wiki/dictionaries/&apos;&gt;here&lt;/a&gt;.&lt;/p&gt;&lt;p&gt;To install a dictionary, go to %1.&lt;/p&gt;</source>
        <translation>&lt;p&gt;要使用搜索，请安装词典。&lt;/p&gt;&lt;p&gt;可在&lt;a href=&apos;https://yomitan.wiki/dictionaries/&apos;&gt;此处&lt;/a&gt;找到词典。&lt;/p&gt;&lt;p&gt;要安装词典，请前往 %1。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="338"/>
        <source>Memento → Preferences → Dictionary</source>
        <translation>Memento → 偏好设置 → 词典</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="339"/>
        <source>Settings → Options → Dictionary</source>
        <translation>设置 → 选项 → 词典</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="360"/>
        <source>Auto Update</source>
        <translation>自动更新</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="363"/>
        <source>Do you want to check for updates on launch?

This can be disabled at any time by going to %1.
This will check GitHub&apos;s API when Memento is launched.</source>
        <translation>是否在启动时检查更新？

可随时前往 %1 禁用此功能。
这会在 Memento 启动时检查 GitHub API。</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="368"/>
        <source>Memento → Preferences → Application</source>
        <translation>Memento → 偏好设置 → 应用程序</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="369"/>
        <source>Settings → Options → Application</source>
        <translation>设置 → 选项 → 应用程序</translation>
    </message>
</context>
<context>
    <name>ManualSearchPage</name>
    <message>
        <location filename="../qml/controls/ManualSearchPage.qml" line="31"/>
        <source>Search</source>
        <translation>搜索</translation>
    </message>
</context>
<context>
    <name>OcrPage</name>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="15"/>
        <source>Select Custom Open Folder</source>
        <translation>选择自定义打开文件夹</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="73"/>
        <source>OCR</source>
        <translation>OCR</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="83"/>
        <source>Enabled</source>
        <translation>已启用</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="100"/>
        <source>Use GPU acceleration</source>
        <translation>使用 GPU 加速</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="117"/>
        <source>Use model</source>
        <translation>使用模型</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="122"/>
        <source>Hugging Face Model</source>
        <translation>Hugging Face 模型</translation>
    </message>
</context>
<context>
    <name>OptionsWindow</name>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="12"/>
        <source>Options</source>
        <translation>选项</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="25"/>
        <source>Anki Integration</source>
        <translation>Anki 集成</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="30"/>
        <source>Application</source>
        <translation>应用程序</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="35"/>
        <source>Audio Sources</source>
        <translation>音频源</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="40"/>
        <source>Behavior</source>
        <translation>行为</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="45"/>
        <source>Dictionary</source>
        <translation>词典</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="50"/>
        <source>Interface</source>
        <translation>界面</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="55"/>
        <source>Keybinds</source>
        <translation>快捷键</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="60"/>
        <source>Search</source>
        <translation>搜索</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="70"/>
        <source>OCR</source>
        <translation>OCR</translation>
    </message>
</context>
<context>
    <name>Player</name>
    <message>
        <location filename="../qml/controls/Player.qml" line="142"/>
        <source>OSC Visibility: %1</source>
        <translation>OSC 可见性：%1</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="148"/>
        <source>Visible</source>
        <translation>可见</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="153"/>
        <source>Hidden</source>
        <translation>隐藏</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="158"/>
        <source>Auto</source>
        <translation>自动</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="261"/>
        <source>Playing media</source>
        <translation>正在播放媒体</translation>
    </message>
</context>
<context>
    <name>PlayerMenu</name>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="43"/>
        <source>Track %1</source>
        <translation>轨道 %1</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="73"/>
        <source>&amp;Media</source>
        <translation>媒体(&amp;M)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="77"/>
        <source>&amp;Open File...</source>
        <translation>打开文件(&amp;O)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="91"/>
        <source>&amp;Open URL...</source>
        <translation>打开 URL(&amp;U)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="106"/>
        <source>&amp;Recent</source>
        <translation>最近(&amp;R)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="136"/>
        <source>&amp;Clear Recents</source>
        <translation>清除最近项目(&amp;C)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="144"/>
        <source>&amp;Audio</source>
        <translation>音频(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="156"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="313"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="370"/>
        <source>None</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="190"/>
        <source>&amp;Subtitle</source>
        <translation>字幕(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="196"/>
        <source>&amp;Auto Pause</source>
        <translation>自动暂停(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="210"/>
        <source>&amp;Size and Position</source>
        <translation>大小和位置(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="214"/>
        <source>&amp;Increase Size</source>
        <translation>增大大小(&amp;I)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="230"/>
        <source>&amp;Decrease Size</source>
        <translation>减小大小(&amp;D)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="246"/>
        <source>&amp;Move Up</source>
        <translation>上移(&amp;U)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="262"/>
        <source>&amp;Move Down</source>
        <translation>下移(&amp;D)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="279"/>
        <source>&amp;Show Subtitles</source>
        <translation>显示字幕(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="296"/>
        <source>&amp;Add Subtitle...</source>
        <translation>添加字幕(&amp;A)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="302"/>
        <source>&amp;Second Track</source>
        <translation>第二轨(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="405"/>
        <source>&amp;Tools</source>
        <translation>工具(&amp;T)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="409"/>
        <source>&amp;Show Search</source>
        <translation>显示搜索(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="425"/>
        <source>&amp;Show Subtitle List</source>
        <translation>显示字幕列表(&amp;L)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="445"/>
        <source>&amp;Start OCR</source>
        <translation>开始 OCR(&amp;O)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="476"/>
        <source>&amp;Settings</source>
        <translation>设置(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="481"/>
        <source>&amp;Anki Profile</source>
        <translation>Anki 配置(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="518"/>
        <source>&amp;Options</source>
        <translation>选项(&amp;O)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="524"/>
        <source>&amp;Open Config</source>
        <translation>打开配置文件夹(&amp;C)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="531"/>
        <source>&amp;Check for Updates</source>
        <translation>检查更新(&amp;U)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="536"/>
        <source>&amp;About Memento</source>
        <translation>关于 Memento(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="546"/>
        <source>Video Files (%1)</source>
        <translation>视频文件 (%1)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="547"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="581"/>
        <source>All Files (*.*)</source>
        <translation>所有文件 (*.*)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="558"/>
        <source>Open Stream</source>
        <translation>打开流</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="570"/>
        <source>Enter URL</source>
        <translation>输入 URL</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="580"/>
        <source>Subtitle Files (%1)</source>
        <translation>字幕文件 (%1)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="583"/>
        <source>Select Subtitle File</source>
        <translation>选择字幕文件</translation>
    </message>
</context>
<context>
    <name>SearchPage</name>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="64"/>
        <source>Matcher</source>
        <translation>匹配器</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="74"/>
        <source>Exact</source>
        <translation>精确</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="91"/>
        <source>Deconjugation</source>
        <translation>活用还原</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="110"/>
        <source>MeCab IPAdic</source>
        <translation>MeCab IPAdic</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="125"/>
        <source>Popup</source>
        <translation>弹出窗口</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="135"/>
        <source>Search method</source>
        <translation>搜索方式</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="142"/>
        <source>Hover</source>
        <translation>悬停</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="146"/>
        <source>Modifier</source>
        <translation>修饰键</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="165"/>
        <source>Search hover millisecond delay</source>
        <translation>悬停搜索延迟（毫秒）</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="185"/>
        <source>Search modifier key</source>
        <translation>搜索修饰键</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="192"/>
        <source>Alt</source>
        <translation>Alt</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="196"/>
        <source>Control</source>
        <translation>Control</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="200"/>
        <source>Shift</source>
        <translation>Shift</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="204"/>
        <source>Super</source>
        <translation>Super</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="223"/>
        <source>Search using middle mouse button</source>
        <translation>使用鼠标中键搜索</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="240"/>
        <source>Glossary style</source>
        <translation>释义样式</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="247"/>
        <source>Bullet Points</source>
        <translation>项目符号</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="251"/>
        <source>Line Breaks</source>
        <translation>换行</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="255"/>
        <source>Pipes</source>
        <translation>竖线</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="274"/>
        <source>Auto play first result audio</source>
        <translation>自动播放第一个结果的音频</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="291"/>
        <source>Show add to Anki glossary checkbox</source>
        <translation>显示添加到 Anki 释义复选框</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="307"/>
        <source>Subtitles</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="317"/>
        <source>Hide mpv subtitles while Memento subtitles are visible</source>
        <translation>Memento 字幕可见时隐藏 mpv 字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="334"/>
        <source>Hide Memento subtitles while playing media</source>
        <translation>播放媒体时隐藏 Memento 字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="351"/>
        <source>Pause when hovering on subtitles</source>
        <translation>悬停在字幕上时暂停</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="368"/>
        <source>Replace new lines in subtitles</source>
        <translation>替换字幕中的换行</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="385"/>
        <source>New line replacement text</source>
        <translation>换行替换文本</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="402"/>
        <source>Remove subtitle text matching regex</source>
        <translation>移除匹配正则表达式的字幕文本</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="406"/>
        <source>Regex</source>
        <translation>正则表达式</translation>
    </message>
</context>
<context>
    <name>SubtitleListPage</name>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="65"/>
        <source>Primary</source>
        <translation>主要</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="72"/>
        <source>Secondary</source>
        <translation>次要</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="233"/>
        <source>Ignore Whitespace</source>
        <translation>忽略空白</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="243"/>
        <source>Auto Seek</source>
        <translation>自动跳转</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="256"/>
        <source>Search</source>
        <translation>搜索</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="285"/>
        <source>No Matches</source>
        <translation>无匹配项</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="286"/>
        <source>%1 of %2</source>
        <translation>%1 / %2</translation>
    </message>
</context>
<context>
    <name>TermEntry</name>
    <message>
        <location filename="../qml/definition/TermEntry.qml" line="86"/>
        <source>Error Adding Anki Note</source>
        <translation>添加 Anki 笔记时出错</translation>
    </message>
    <message>
        <location filename="../qml/definition/TermEntry.qml" line="251"/>
        <source>Could Not Open Anki</source>
        <translation>无法打开 Anki</translation>
    </message>
</context>
<context>
    <name>UpdateDialog</name>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="27"/>
        <source>Update Check Failed</source>
        <translation>更新检查失败</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="28"/>
        <source>&lt;p&gt;The GitHub API did not return a valid reply.&lt;/p&gt;&lt;p&gt;Check for updates manually &lt;a href=&quot;%1&quot;&gt;here&lt;/a&gt;.</source>
        <translation>&lt;p&gt;GitHub API 未返回有效响应。&lt;/p&gt;&lt;p&gt;请&lt;a href=&quot;%1&quot;&gt;在此处&lt;/a&gt;手动检查更新。</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="65"/>
        <source>Update to Date</source>
        <translation>已是最新</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="66"/>
        <source>&lt;p&gt;You&apos;re on the latest version.&lt;/p&gt;</source>
        <translation>&lt;p&gt;您正在使用最新版本。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="74"/>
        <source>New Version Available</source>
        <translation>有新版本可用</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="75"/>
        <source>&lt;p&gt;Version &lt;a href=&quot;%1&quot;&gt;%2&lt;/a&gt; is available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;版本 &lt;a href=&quot;%1&quot;&gt;%2&lt;/a&gt; 可用。&lt;/p&gt;</translation>
    </message>
</context>
</TS>
