<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_TW">
<context>
    <name>AboutWindow</name>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="8"/>
        <source>About Memento</source>
        <translation>關於 Memento</translation>
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
        <translation>僅限 GPLv2</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="50"/>
        <source>&lt;p&gt;&lt;a href=&quot;https://ripose-jp.github.io/Memento/&quot;&gt;Project Site&lt;/a&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;a href=&quot;https://ripose-jp.github.io/Memento/&quot;&gt;專案網站&lt;/a&gt;&lt;/p&gt;</translation>
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
        <translation>Anki 整合說明</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="34"/>
        <source>Introduction</source>
        <translation>介紹</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="40"/>
        <source>This document describes the syntax and the associated markers available for use in note templates.</source>
        <translation>本文件介紹可在筆記範本中使用的語法和相關標記。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="47"/>
        <source>Syntax</source>
        <translation>語法</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="54"/>
        <source>&lt;p&gt;A marker is a section of text enclosed in curly braces like &lt;code&gt;{subtitle}&lt;/code&gt;. When Memento&apos;s template processor sees a marker, it replaces it with its associated value. In the case of &lt;code&gt;{subtitle}&lt;/code&gt; that is the currently displayed subtitle.&lt;/p&gt;&lt;p&gt;Some markers have arguments that allow their functionality to modified. For example, &lt;code&gt;{glossary : dict = 1}&lt;/code&gt; makes it so the glossary marker only uses entries from the dictionary with the ID of 1. The &lt;code&gt;:&lt;/code&gt; character indicates the beginning of marker arguments. Arguments are set in the form of &lt;code&gt;name = value&lt;/code&gt;. If a marker has multiple arguments, arguments are separated by the &lt;code&gt;,&lt;/code&gt; character. An example of a marker with multiple arguments is &lt;code&gt;{screenshot : max-width = 1280, max-height = 720}&lt;/code&gt;.&lt;/p&gt;&lt;p&gt;A single marker can specify whether to use one or another value. For example, &lt;code&gt;{selection | clipboard}&lt;/code&gt; specifies if &lt;code&gt;selection&lt;/code&gt; is empty, use &lt;code&gt;clipboard&lt;/code&gt; instead. This functionality is triggered by the &lt;code&gt;|&lt;/code&gt; character. Multiple ors can be chained together like &lt;code&gt;{selection | clipboard | glossary : dict = 1}&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;標記是一段以大括號包圍的文字，例如 &lt;code&gt;{subtitle}&lt;/code&gt;。Memento 的範本處理器遇到標記時，會將標記替換為對應的值。以 &lt;code&gt;{subtitle}&lt;/code&gt; 為例，對應值就是目前顯示的字幕。&lt;/p&gt;&lt;p&gt;部分標記可使用參數調整功能。例如，&lt;code&gt;{glossary : dict = 1}&lt;/code&gt; 會讓釋義標記只使用 ID 為 1 的詞典項目。&lt;code&gt;:&lt;/code&gt; 字元表示標記參數的開頭。參數以 &lt;code&gt;name = value&lt;/code&gt; 格式設定。若標記有多個參數，請以 &lt;code&gt;,&lt;/code&gt; 字元分隔。例如：&lt;code&gt;{screenshot : max-width = 1280, max-height = 720}&lt;/code&gt;。&lt;/p&gt;&lt;p&gt;單一標記也可以指定替代值。例如，&lt;code&gt;{selection | clipboard}&lt;/code&gt; 表示當 &lt;code&gt;selection&lt;/code&gt; 為空時改用 &lt;code&gt;clipboard&lt;/code&gt;。此功能由 &lt;code&gt;|&lt;/code&gt; 字元觸發，也可以像 &lt;code&gt;{selection | clipboard | glossary : dict = 1}&lt;/code&gt; 一樣串接多個替代項目。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="82"/>
        <source>Term Markers</source>
        <translation>詞條標記</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="88"/>
        <source>This section describes the markers available for use in term notes.</source>
        <translation>本節介紹可在詞條筆記中使用的標記。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="111"/>
        <source>Kanji Markers</source>
        <translation>漢字標記</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="117"/>
        <source>This section describes the markers available for use in kanji notes.</source>
        <translation>本節介紹可在漢字筆記中使用的標記。</translation>
    </message>
</context>
<context>
    <name>AnkiIntegrationPage</name>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="89"/>
        <source>Add Profile</source>
        <translation>新增設定檔</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="95"/>
        <source>Profile name</source>
        <translation>設定檔名稱</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="101"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="106"/>
        <source>Copy current profile</source>
        <translation>複製目前設定檔</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="206"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="216"/>
        <source>Enable Anki integration</source>
        <translation>啟用 Anki 整合</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="233"/>
        <source>Show advanced settings</source>
        <translation>顯示進階設定</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="247"/>
        <source>Profile</source>
        <translation>設定檔</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="257"/>
        <source>Current profile</source>
        <translation>目前設定檔</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="304"/>
        <source>A profile with this name already exists</source>
        <translation>已存在同名設定檔</translation>
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
        <translation>主機名稱</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="353"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="357"/>
        <source>Port</source>
        <translation>連接埠</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="375"/>
        <source>Use API key</source>
        <translation>使用 API 金鑰</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="395"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="399"/>
        <source>API key</source>
        <translation>API 金鑰</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="413"/>
        <source>Test connection</source>
        <translation>測試連線</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="417"/>
        <source>Connect</source>
        <translation>連線</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="424"/>
        <source>Anki Connection</source>
        <translation>Anki 連線</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="425"/>
        <source>Connection successful.</source>
        <translation>連線成功。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="430"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="714"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="751"/>
        <source>Anki Connection Error</source>
        <translation>Anki 連線錯誤</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="431"/>
        <source>Failed to connect to Anki: %1.</source>
        <translation>無法連線到 Anki：%1。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="447"/>
        <source>Advanced</source>
        <translation>進階</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="457"/>
        <source>Duplicate policy</source>
        <translation>重複處理策略</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="464"/>
        <source>None</source>
        <translation>無</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="468"/>
        <source>Allowed in different decks</source>
        <translation>允許在不同牌組中重複</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="472"/>
        <source>Allowed in same deck</source>
        <translation>允許在同一牌組中重複</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="491"/>
        <source>Replace newlines with</source>
        <translation>將換行取代為</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="495"/>
        <source>Replacement string</source>
        <translation>取代字串</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="509"/>
        <source>Screenshot file type</source>
        <translation>螢幕截圖檔案類型</translation>
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
        <translation>音訊開頭填補時間</translation>
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
        <translation>音訊結尾填補時間</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="599"/>
        <source>Audio media normalization</source>
        <translation>音訊媒體正規化</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="616"/>
        <source>Audio normalization dB</source>
        <translation>音訊正規化 dB</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="621"/>
        <source>dB</source>
        <translation>dB</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="642"/>
        <source>Include in glossary</source>
        <translation>包含在釋義中</translation>
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
        <translation>標籤</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="696"/>
        <source>Term Notes</source>
        <translation>詞條筆記</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="715"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="752"/>
        <source>Could not get model fields: %1.</source>
        <translation>無法取得模型欄位：%1。</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="733"/>
        <source>Kanji Notes</source>
        <translation>漢字筆記</translation>
    </message>
</context>
<context>
    <name>AnkiMarkerDescription</name>
    <message>
        <location filename="../qml/controls/AnkiMarkerDescription.qml" line="52"/>
        <source>&lt;p&gt;&lt;b&gt;Default Value:&lt;/b&gt; %1&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;預設值：&lt;/b&gt;%1&lt;/p&gt;</translation>
    </message>
</context>
<context>
    <name>AnkiMarkers</name>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="48"/>
        <source>&lt;p&gt;Audio of the subtitle list selection.&lt;/p&gt;</source>
        <translation>&lt;p&gt;字幕清單所選內容的音訊。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="51"/>
        <source>&lt;p&gt;Audio of the word if found and the skip hash is not matched.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果找到單字且略過雜湊不相符，則為該單字的音訊。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="54"/>
        <source>&lt;p&gt;Audio of the current subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;目前字幕的音訊。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="57"/>
        <source>&lt;p&gt;The kanji character.&lt;/p&gt;</source>
        <translation>&lt;p&gt;漢字字元。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="60"/>
        <source>&lt;p&gt;The current text in the clipboard.&lt;/p&gt;</source>
        <translation>&lt;p&gt;剪貼簿中的目前文字。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="63"/>
        <source>&lt;p&gt;The raw expression as recognized by Memento.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Memento 辨識到的原始詞條。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="66"/>
        <source>&lt;p&gt;The text before &lt;code&gt;{%1}&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;code&gt;{%1}&lt;/code&gt; 之前的文字。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="69"/>
        <source>&lt;p&gt;The text after &lt;code&gt;{%1}&lt;/code&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;code&gt;{%1}&lt;/code&gt; 之後的文字。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="72"/>
        <source>&lt;p&gt;Text of all selected lines in the secondary subtitle list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;次要字幕清單中所有選取行的文字。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="75"/>
        <source>&lt;p&gt;Text of all selected lines in the primary subtitle list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;主要字幕清單中所有選取行的文字。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="78"/>
        <source>&lt;p&gt;The expression in kanji. Kana if there is no kanji or explicitly requested.&lt;/p&gt;</source>
        <translation>&lt;p&gt;漢字形式的詞條。沒有漢字或明確要求時則使用假名。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="81"/>
        <source>&lt;p&gt;The average of frequency data for the expression. Defaults to 0 occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;該詞條頻率資料的平均值。沒有頻率資料時，出現次數預設為 0。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="85"/>
        <source>&lt;p&gt;The average of frequency data for the expression. Defaults to 9999999 when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;該詞條頻率資料的平均值。沒有頻率資料時預設為 9999999。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="89"/>
        <source>&lt;p&gt;The harmonic mean of frequency data for the expression. Defaults to 0 for occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;該詞條頻率資料的調和平均值。沒有頻率資料時，出現次數預設為 0。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="93"/>
        <source>&lt;p&gt;The harmonic mean of frequency data for the expression. Defaults to 9999999 for occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;該詞條頻率資料的調和平均值。沒有頻率資料時，出現次數預設為 9999999。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="97"/>
        <source>&lt;p&gt;Bulleted list of frequency tags.&lt;/p&gt;</source>
        <translation>&lt;p&gt;頻率標籤的項目符號清單。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="100"/>
        <source>&lt;p&gt;The expression in kanji with furigana above. Kana if there is no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;漢字詞條上方附有振假名。沒有漢字時則使用假名。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="104"/>
        <source>&lt;p&gt;The expression in kanji with furigana proceeding in brackets. Kana if there is no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;漢字詞條後方以括號標示振假名。沒有漢字時則使用假名。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="108"/>
        <source>&lt;p&gt;Dictionary definitions of the expression without dictionary information.&lt;/p&gt;</source>
        <translation>&lt;p&gt;不含詞典資訊的詞條釋義。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="111"/>
        <source>&lt;p&gt;Dictionary definitions of the expression in compact form.&lt;/p&gt;</source>
        <translation>&lt;p&gt;精簡形式的詞條釋義。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="114"/>
        <source>&lt;p&gt;Dictionary definitions of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;詞條的詞典釋義。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="117"/>
        <source>&lt;p&gt;A comma separated list of kunyomi (Japanese) readings written in katakana.&lt;/p&gt;</source>
        <translation>&lt;p&gt;以逗號分隔、使用片假名書寫的訓讀（日語讀法）清單。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="120"/>
        <source>&lt;p&gt;A comma separated list of onyomi (Chinese) readings written in hiragana.&lt;/p&gt;</source>
        <translation>&lt;p&gt;以逗號分隔的音讀（漢語讀法）清單，使用平假名書寫。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="123"/>
        <source>&lt;p&gt;A comma separated list of pitch accent categories for the expression: heiban, kifuku, atamadaka, nakadaka, odaka.&lt;/p&gt;</source>
        <translation>&lt;p&gt;以逗號分隔的詞條音高重音類別清單：heiban、kifuku、atamadaka、nakadaka、odaka。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="127"/>
        <source>&lt;p&gt;Pitch accents for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;詞條讀音的音高重音。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="130"/>
        <source>&lt;p&gt;Pitch graphs for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;詞條讀音的音高圖。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="133"/>
        <source>&lt;p&gt;Pitch positions for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;詞條讀音的音高位置。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="136"/>
        <source>&lt;p&gt;The reading of the expression in kana. Empty if the expression has no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;以假名表示的詞條讀音。詞條沒有漢字時為空。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="140"/>
        <source>&lt;p&gt;Screenshot of the current frame.&lt;/p&gt;</source>
        <translation>&lt;p&gt;目前影格的螢幕截圖。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="143"/>
        <source>&lt;p&gt;Screenshot of the current frame without subtitles if visible.&lt;/p&gt;</source>
        <translation>&lt;p&gt;目前影格的螢幕截圖；如果字幕可見，則不包含字幕。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="146"/>
        <source>&lt;p&gt;The currently selected glossary text. If multiple glossaries are selected, they are ordered as a list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;目前選取的釋義文字。如果選取多個釋義，會依序排列為清單。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="150"/>
        <source>&lt;p&gt;The number of strokes in the character.&lt;/p&gt;</source>
        <translation>&lt;p&gt;字元的筆畫數。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="153"/>
        <source>&lt;p&gt;The text of the secondary subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;次要字幕的文字。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="156"/>
        <source>&lt;p&gt;The text of the primary subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;主要字幕的文字。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="159"/>
        <source>&lt;p&gt;Unordered list of expression tags without descriptions.&lt;/p&gt;</source>
        <translation>&lt;p&gt;不含說明的詞條標籤無序清單。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="162"/>
        <source>&lt;p&gt;Unordered list of expression tags.&lt;/p&gt;</source>
        <translation>&lt;p&gt;詞條標籤的無序清單。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="165"/>
        <source>&lt;p&gt;Title of the current media. Filename if no title is set.&lt;/p&gt;</source>
        <translation>&lt;p&gt;目前媒體的標題。未設定標題時為檔案名稱。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="168"/>
        <source>&lt;p&gt;An MP4 file containing a clip of the current context.&lt;/p&gt;</source>
        <translation>&lt;p&gt;包含目前情境片段的 MP4 檔案。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="184"/>
        <source>&lt;p&gt;If true, audio is included in the clip.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果為 true，則片段包含音訊。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="187"/>
        <source>&lt;p&gt;The decibel value to normalize audio to. Ignored if &lt;code&gt;%1 = false&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;音訊正規化的目標分貝值。如果 &lt;code&gt;%1 = false&lt;/code&gt;，則忽略此設定。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="191"/>
        <source>&lt;p&gt;Filter to only use entries from a particular dictionary. Takes a dictionary ID. Dictionary IDs can be found in &lt;b&gt;Options&lt;/b&gt; under &lt;b&gt;Dictionaries&lt;/b&gt; at the end of dictionary names.&lt;/p&gt;</source>
        <translation>&lt;p&gt;篩選為只使用特定詞典中的項目。接受詞典 ID。詞典 ID 可在&lt;b&gt;選項&lt;/b&gt;的&lt;b&gt;詞典&lt;/b&gt;中，在詞典名稱結尾找到。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="196"/>
        <source>&lt;p&gt;If true, maintains the aspect ratio of the image during resize.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果為 true，調整大小時保持影像長寬比。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="199"/>
        <source>&lt;p&gt;Used to determine the maximum height of a saved screenshot. A value of -1 leaves this unbounded.&lt;/p&gt;</source>
        <translation>&lt;p&gt;用於決定已儲存螢幕截圖的最大高度。值為 -1 表示不限制。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="203"/>
        <source>&lt;p&gt;Used to determine the maximum width of a saved screenshot. A value of -1 leaves this unbounded.&lt;/p&gt;</source>
        <translation>&lt;p&gt;用於決定已儲存螢幕截圖的最大寬度。值為 -1 表示不限制。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="207"/>
        <source>&lt;p&gt;If true, only export the lowest frequency value.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果為 true，只匯出最低頻率值。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="210"/>
        <source>&lt;p&gt;If true, normalize audio.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果為 true，則將音訊正規化。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="213"/>
        <source>&lt;p&gt;The source of clip timings. Possible values are:&lt;ul&gt;&lt;li&gt;&lt;b&gt;subtitle&lt;/b&gt;: Use timings from the current subtitle.&lt;/li&gt;&lt;li&gt;&lt;b&gt;context&lt;/b&gt;: Use timings from the currently selected context.&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;片段時間的來源。可用值為：&lt;ul&gt;&lt;li&gt;&lt;b&gt;subtitle&lt;/b&gt;：使用目前字幕的時間。&lt;/li&gt;&lt;li&gt;&lt;b&gt;context&lt;/b&gt;：使用目前所選情境的時間。&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="222"/>
        <source>&lt;p&gt;If true, subtitles are included in the clip.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果為 true，則片段包含字幕。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="225"/>
        <source>&lt;p&gt;If true, export frequency values without dictionary names.&lt;/p&gt;</source>
        <translation>&lt;p&gt;如果為 true，匯出不含詞典名稱的頻率值。&lt;/p&gt;</translation>
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
        <translation>設定檔的正規化 dB 設定</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="231"/>
        <source>None</source>
        <translation>無</translation>
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
        <translation>設定檔的音訊媒體正規化設定</translation>
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
        <translation>牌組</translation>
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
        <translation>語言</translation>
    </message>
    <message>
        <source>Interface language</source>
        <translation type="vanished">介面語言</translation>
    </message>
    <message>
        <source>Interface language (requires restart)</source>
        <translation type="vanished">介面語言（需要重新啟動）</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="76"/>
        <source>Interface language (Restart required)</source>
        <translation>介面語言（需要重新啟動）</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="83"/>
        <source>System</source>
        <translation>系統</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="118"/>
        <source>Update</source>
        <translation>更新</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="128"/>
        <source>Check for updates on launch</source>
        <translation>啟動時檢查更新</translation>
    </message>
</context>
<context>
    <name>AudioSourcesPage</name>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="65"/>
        <source>Audio Sources Help</source>
        <translation>音訊來源說明</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="70"/>
        <source>&lt;p&gt;&lt;b&gt;Source Name&lt;/b&gt;: The name of the audio source as it will appear in Memento.&lt;/p&gt;
                        &lt;p&gt;&lt;b&gt;URL&lt;/b&gt;: The URL of the audio source. Supports inserting &lt;b&gt;{expression}&lt;/b&gt; and
                            &lt;b&gt;{reading}&lt;/b&gt; markers into the URL.&lt;/p&gt;
                        &lt;p&gt;&lt;b&gt;MD5 Skip Hash&lt;/b&gt;: Audio that matches this MD5 hash will be ignored.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;來源名稱&lt;/b&gt;：音訊來源在 Memento 中顯示的名稱。&lt;/p&gt;&lt;p&gt;&lt;b&gt;URL&lt;/b&gt;：音訊來源的 URL。支援在 URL 中插入 &lt;b&gt;{expression}&lt;/b&gt; 和 &lt;b&gt;{reading}&lt;/b&gt; 標記。&lt;/p&gt;&lt;p&gt;&lt;b&gt;MD5 略過雜湊&lt;/b&gt;：與此 MD5 雜湊相符的音訊會被忽略。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="112"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="119"/>
        <source>URL</source>
        <translation>URL</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="128"/>
        <source>File</source>
        <translation>檔案</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="132"/>
        <source>JSON</source>
        <translation>JSON</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="143"/>
        <source>MD5 Skip Hash</source>
        <translation>MD5 略過雜湊</translation>
    </message>
</context>
<context>
    <name>BehaviorPage</name>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="15"/>
        <source>Select Custom Open Folder</source>
        <translation>選擇自訂開啟資料夾</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="72"/>
        <source>Player</source>
        <translation>播放器</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="82"/>
        <source>Auto fit media on load</source>
        <translation>載入媒體時自動調整視窗大小</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="99"/>
        <source>Auto fit media percentage</source>
        <translation>視窗自動調整比例</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="117"/>
        <source>On Screen Controller</source>
        <translation>螢幕控制器</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="127"/>
        <source>Visibility duration milliseconds</source>
        <translation>可見持續時間（毫秒）</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="147"/>
        <source>Fade duration milliseconds</source>
        <translation>淡出持續時間（毫秒）</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="167"/>
        <source>Minimum mouse pixel movement</source>
        <translation>最小滑鼠移動像素</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="187"/>
        <source>Allow mpv to control cursor visibility</source>
        <translation>允許 mpv 控制游標可見性</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="204"/>
        <source>Show preview thumbnails</source>
        <translation>顯示預覽縮圖</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="219"/>
        <source>Subtitles</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="229"/>
        <source>Automatically pause at the end of subtitles</source>
        <translation>在字幕結尾自動暫停</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="246"/>
        <source>Show subtitles when the cursor is at the bottom of the player</source>
        <translation>游標在播放器底部時顯示字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="263"/>
        <source>Show secondary subtitles when the cursor is at the top of the player</source>
        <translation>游標在播放器頂端時顯示次要字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="279"/>
        <source>Open File Directory</source>
        <translation>開啟檔案目錄</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="289"/>
        <source>Open file starting location</source>
        <translation>開啟檔案起始位置</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="296"/>
        <source>Current</source>
        <translation>目前</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="300"/>
        <source>Custom</source>
        <translation>自訂</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="304"/>
        <source>Documents</source>
        <translation>文件</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="308"/>
        <source>Home</source>
        <translation>主資料夾</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="312"/>
        <source>Movies</source>
        <translation>影片</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="331"/>
        <source>Custom file directory</source>
        <translation>自訂檔案目錄</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="335"/>
        <source>Browse</source>
        <translation>瀏覽</translation>
    </message>
</context>
<context>
    <name>DatabaseManager</name>
    <message>
        <location filename="../dict/databasemanager.cpp" line="395"/>
        <location filename="../dict/databasemanager.cpp" line="526"/>
        <source>Could not prepare database query</source>
        <translation>無法準備資料庫查詢</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="404"/>
        <location filename="../dict/databasemanager.cpp" line="416"/>
        <location filename="../dict/databasemanager.cpp" line="428"/>
        <location filename="../dict/databasemanager.cpp" line="534"/>
        <source>Could not bind values to statement</source>
        <translation>無法將值繫結到陳述式</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="463"/>
        <source>Error when executing sqlite query. Code %1</source>
        <translation>執行 SQLite 查詢時發生錯誤。錯誤碼 %1</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="473"/>
        <source>Error getting term information</source>
        <translation>取得詞條資訊時發生錯誤</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="608"/>
        <source>Error while executing kanji query</source>
        <translation>執行漢字查詢時發生錯誤</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1105"/>
        <source>Could not add index.json</source>
        <translation>無法新增 index.json</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1107"/>
        <source>Could not add kanji terms</source>
        <translation>無法新增漢字詞條</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1109"/>
        <source>Could not add kanji metadata</source>
        <translation>無法新增漢字中繼資料</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1111"/>
        <source>Could not add tags</source>
        <translation>無法新增標籤</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1113"/>
        <source>Could not add terms</source>
        <translation>無法新增詞條</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1115"/>
        <source>Could not add term metadata</source>
        <translation>無法新增詞條中繼資料</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1117"/>
        <source>Could not open database</source>
        <translation>無法開啟資料庫</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1119"/>
        <source>Database is of a newer version</source>
        <translation>資料庫版本較新</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1121"/>
        <source>Could not open dictionary file</source>
        <translation>無法開啟詞典檔案</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1123"/>
        <source>Could not execute delete query on database</source>
        <translation>無法在資料庫上執行刪除查詢</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1125"/>
        <source>Could not extract dictionary resources</source>
        <translation>無法擷取詞典資源</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1127"/>
        <source>Could not remove dictionary resources</source>
        <translation>無法移除詞典資源</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1129"/>
        <source>Unknown error</source>
        <translation>未知錯誤</translation>
    </message>
</context>
<context>
    <name>Deconjugator</name>
    <message>
        <location filename="../dict/deconjugator.cpp" line="475"/>
        <source>godan verb</source>
        <translation>五段動詞</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="477"/>
        <source>ichidan verb</source>
        <translation>一段動詞</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="479"/>
        <source>suru verb</source>
        <translation>する 動詞</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="481"/>
        <source>kuru verb</source>
        <translation>くる 動詞</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="483"/>
        <source>adjective</source>
        <translation>形容詞</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="485"/>
        <source>negative</source>
        <translation>否定</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="487"/>
        <source>past</source>
        <translation>過去</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="489"/>
        <source>-te</source>
        <translation>て形</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="491"/>
        <source>masu stem</source>
        <translation>ます詞幹</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="493"/>
        <source>volitional</source>
        <translation>意志形</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="495"/>
        <source>passive</source>
        <translation>被動</translation>
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
        <translation>可能或被動</translation>
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
        <translation>禮貌</translation>
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
        <translation>口語否定</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="545"/>
        <source>provisional colloquial negative</source>
        <translation>條件口語否定</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="547"/>
        <source>progressive or perfect</source>
        <translation>進行或完成</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="549"/>
        <source>adv</source>
        <translation>副詞</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="551"/>
        <source>noun</source>
        <translation>名詞</translation>
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
        <translation>詞典檔案 (*.zip)</translation>
    </message>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="18"/>
        <source>Select Dictionaries</source>
        <translation>選擇詞典</translation>
    </message>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="33"/>
        <source>Dictionary Error</source>
        <translation>詞典錯誤</translation>
    </message>
</context>
<context>
    <name>InterfacePage</name>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="66"/>
        <source>Theme</source>
        <translation>佈景主題</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="76"/>
        <source>Use system icons</source>
        <translation>使用系統圖示</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="92"/>
        <source>Subtitle</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="102"/>
        <source>Font</source>
        <translation>字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="111"/>
        <source>Select Subtitle Font</source>
        <translation>選擇字幕字型</translation>
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
        <translation>粗體</translation>
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
        <translation>斜體</translation>
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
        <translation>行高占視窗高度的百分比</translation>
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
        <translation>底部偏移占視窗高度的百分比</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="233"/>
        <source>Stroke size</source>
        <translation>描邊大小</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="256"/>
        <location filename="../qml/options/InterfacePage.qml" line="1018"/>
        <source>Text color</source>
        <translation>文字顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="269"/>
        <source>Select Subtitle Text Color</source>
        <translation>選擇字幕文字顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="284"/>
        <location filename="../qml/options/InterfacePage.qml" line="990"/>
        <source>Background color</source>
        <translation>背景顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="297"/>
        <source>Select Subtitle Background Color</source>
        <translation>選擇字幕背景顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="312"/>
        <source>Stroke color</source>
        <translation>描邊顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="325"/>
        <source>Select Subtitle Stroke Color</source>
        <translation>選擇字幕描邊顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="370"/>
        <source>Search</source>
        <translation>搜尋</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="380"/>
        <source>Term Expression Font</source>
        <translation>詞條表記字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="389"/>
        <source>Select Term Expression Font</source>
        <translation>選擇詞條表記字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="445"/>
        <source>Term Reading Font</source>
        <translation>詞條讀音字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="454"/>
        <source>Select Term Reading Font</source>
        <translation>選擇詞條讀音字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="510"/>
        <source>Term Conjugation Explanation Font</source>
        <translation>詞條活用說明字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="519"/>
        <source>Select Term Conjugation Explanation Font</source>
        <translation>選擇詞條活用說明字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="575"/>
        <source>Tag Font</source>
        <translation>標籤字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="584"/>
        <source>Select Tag Font</source>
        <translation>選擇標籤字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="640"/>
        <source>Glossary Font</source>
        <translation>釋義字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="649"/>
        <source>Select Glossary Font</source>
        <translation>選擇釋義字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="705"/>
        <source>Kanji Font</source>
        <translation>漢字字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="714"/>
        <source>Select Kanji Font</source>
        <translation>選擇漢字字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="768"/>
        <source>Popup</source>
        <translation>彈出視窗</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="778"/>
        <source>Popup width</source>
        <translation>彈出視窗寬度</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="798"/>
        <source>Popup height</source>
        <translation>彈出視窗高度</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="816"/>
        <source>Subtitle List</source>
        <translation>字幕清單</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="826"/>
        <location filename="../qml/options/InterfacePage.qml" line="1139"/>
        <source>Open in a separate window</source>
        <translation>在單獨視窗中開啟</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="843"/>
        <source>Show timestamps</source>
        <translation>顯示時間戳記</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="860"/>
        <source>Primary subtitle font</source>
        <translation>主要字幕字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="869"/>
        <source>Select Subtitle List Primary Font</source>
        <translation>選擇字幕清單主要字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="925"/>
        <source>Secondary subtitle font</source>
        <translation>次要字幕字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="934"/>
        <source>Select Subtitle List Secondary Font</source>
        <translation>選擇字幕清單次要字型</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1003"/>
        <source>Select Subtitle List Background Color</source>
        <translation>選擇字幕清單背景顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1031"/>
        <source>Select Subtitle List Text Color</source>
        <translation>選擇字幕清單文字顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1046"/>
        <source>Hover color</source>
        <translation>懸停顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1059"/>
        <source>Select Subtitle List Hover Color</source>
        <translation>選擇字幕清單懸停顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1074"/>
        <source>Selection color</source>
        <translation>選取顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1087"/>
        <source>Select Subtitle List Selection Color</source>
        <translation>選擇字幕清單選取顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1102"/>
        <source>Hovered selection color</source>
        <translation>懸停選取顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1115"/>
        <source>Select Subtitle List Hovered Selection Color</source>
        <translation>選擇字幕清單懸停選取顏色</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1129"/>
        <source>Auxiliary Search</source>
        <translation>輔助搜尋</translation>
    </message>
</context>
<context>
    <name>KanjiEntry</name>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="55"/>
        <source>Error Adding Anki Note</source>
        <translation>新增 Anki 筆記時發生錯誤</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="151"/>
        <source>Could Not Open Anki</source>
        <translation>無法開啟 Anki</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="230"/>
        <source>Glossary</source>
        <translation>釋義</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="237"/>
        <source>Reading</source>
        <translation>讀音</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="244"/>
        <source>Statistics</source>
        <translation>統計</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="312"/>
        <source>Classifications</source>
        <translation>分類</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="341"/>
        <source>Codepoints</source>
        <translation>碼位</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="370"/>
        <source>Dictionary Indices</source>
        <translation>詞典索引</translation>
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
        <translation>未繫結</translation>
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
        <translation>新增設定檔</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="59"/>
        <source>Profile name</source>
        <translation>設定檔名稱</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="65"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="70"/>
        <source>Copy current profile</source>
        <translation>複製目前設定檔</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="178"/>
        <source>Add current expression to Anki</source>
        <translation>將目前詞條新增至 Anki</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="193"/>
        <source>Scroll to previous search result</source>
        <translation>捲動到上一個搜尋結果</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="208"/>
        <source>Scroll to next search result</source>
        <translation>捲動到下一個搜尋結果</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="223"/>
        <source>Open file</source>
        <translation>開啟檔案</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="238"/>
        <source>Open URL</source>
        <translation>開啟 URL</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="253"/>
        <source>Cycle OSC visibility</source>
        <translation>循環切換 OSC 可見性</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="268"/>
        <source>Show auxiliary search</source>
        <translation>顯示輔助搜尋</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="283"/>
        <source>Show subtitle list</source>
        <translation>顯示字幕清單</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="300"/>
        <source>Start OCR</source>
        <translation>開始 OCR</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="315"/>
        <source>Toggle subtitle auto pause</source>
        <translation>切換字幕自動暫停</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="330"/>
        <source>Toggle subtitle visibility</source>
        <translation>切換字幕可見性</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="345"/>
        <source>Decrease subtitle size</source>
        <translation>縮小字幕</translation>
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
        <translation>複製字幕清單所選內容</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="420"/>
        <source>Subtitle list find</source>
        <translation>字幕清單查找</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="435"/>
        <source>Subtitle list find next</source>
        <translation>字幕清單查找下一個</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="450"/>
        <source>Subtitle list find previous</source>
        <translation>字幕清單查找上一個</translation>
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
        <translation>搜尋</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="287"/>
        <source>Subtitle List</source>
        <translation>字幕清單</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="327"/>
        <source>No Dictionaries Installed</source>
        <translation>尚未安裝詞典</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="333"/>
        <source>&lt;p&gt;For searching to work, please install a dictionary.&lt;/p&gt;&lt;p&gt;Dictionaries can be found &lt;a href=&apos;https://yomitan.wiki/dictionaries/&apos;&gt;here&lt;/a&gt;.&lt;/p&gt;&lt;p&gt;To install a dictionary, go to %1.&lt;/p&gt;</source>
        <translation>&lt;p&gt;若要使用搜尋功能，請先安裝詞典。&lt;/p&gt;&lt;p&gt;您可以在&lt;a href=&apos;https://yomitan.wiki/dictionaries/&apos;&gt;這裡&lt;/a&gt;找到詞典。&lt;/p&gt;&lt;p&gt;若要安裝詞典，請前往 %1。&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="338"/>
        <source>Memento → Preferences → Dictionary</source>
        <translation>Memento → 偏好設定 → 詞典</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="339"/>
        <source>Settings → Options → Dictionary</source>
        <translation>設定 → 選項 → 詞典</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="360"/>
        <source>Auto Update</source>
        <translation>自動更新</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="363"/>
        <source>Do you want to check for updates on launch?

This can be disabled at any time by going to %1.
This will check GitHub&apos;s API when Memento is launched.</source>
        <translation>是否要在啟動時檢查更新？

您可以隨時前往 %1 停用此功能。
Memento 啟動時將會查詢 GitHub API。</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="368"/>
        <source>Memento → Preferences → Application</source>
        <translation>Memento → 偏好設定 → 應用程式</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="369"/>
        <source>Settings → Options → Application</source>
        <translation>設定 → 選項 → 應用程式</translation>
    </message>
</context>
<context>
    <name>ManualSearchPage</name>
    <message>
        <location filename="../qml/controls/ManualSearchPage.qml" line="31"/>
        <source>Search</source>
        <translation>搜尋</translation>
    </message>
</context>
<context>
    <name>OcrPage</name>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="15"/>
        <source>Select Custom Open Folder</source>
        <translation>選擇自訂開啟資料夾</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="73"/>
        <source>OCR</source>
        <translation>OCR</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="83"/>
        <source>Enabled</source>
        <translation>已啟用</translation>
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
        <translation>選項</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="25"/>
        <source>Anki Integration</source>
        <translation>Anki 整合</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="30"/>
        <source>Application</source>
        <translation>應用程式</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="35"/>
        <source>Audio Sources</source>
        <translation>音訊來源</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="40"/>
        <source>Behavior</source>
        <translation>行為</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="45"/>
        <source>Dictionary</source>
        <translation>詞典</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="50"/>
        <source>Interface</source>
        <translation>界面</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="55"/>
        <source>Keybinds</source>
        <translation>快捷鍵</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="60"/>
        <source>Search</source>
        <translation>搜尋</translation>
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
        <translation>OSC 可見性：%1</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="148"/>
        <source>Visible</source>
        <translation>可見</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="153"/>
        <source>Hidden</source>
        <translation>隱藏</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="158"/>
        <source>Auto</source>
        <translation>自動</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="261"/>
        <source>Playing media</source>
        <translation>正在播放媒體</translation>
    </message>
</context>
<context>
    <name>PlayerMenu</name>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="43"/>
        <source>Track %1</source>
        <translation>軌道 %1</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="73"/>
        <source>&amp;Media</source>
        <translation>媒體(&amp;M)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="77"/>
        <source>&amp;Open File...</source>
        <translation>開啟檔案(&amp;O)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="91"/>
        <source>&amp;Open URL...</source>
        <translation>開啟 URL(&amp;U)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="106"/>
        <source>&amp;Recent</source>
        <translation>最近(&amp;R)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="136"/>
        <source>&amp;Clear Recents</source>
        <translation>清除最近項目(&amp;C)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="144"/>
        <source>&amp;Audio</source>
        <translation>音訊(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="156"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="313"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="370"/>
        <source>None</source>
        <translation>無</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="190"/>
        <source>&amp;Subtitle</source>
        <translation>字幕(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="196"/>
        <source>&amp;Auto Pause</source>
        <translation>自動暫停(&amp;A)</translation>
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
        <translation>縮小(&amp;D)</translation>
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
        <translation>顯示字幕(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="296"/>
        <source>&amp;Add Subtitle...</source>
        <translation>新增字幕(&amp;A)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="302"/>
        <source>&amp;Second Track</source>
        <translation>第二軌(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="405"/>
        <source>&amp;Tools</source>
        <translation>工具(&amp;T)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="409"/>
        <source>&amp;Show Search</source>
        <translation>顯示搜尋(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="425"/>
        <source>&amp;Show Subtitle List</source>
        <translation>顯示字幕清單(&amp;L)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="445"/>
        <source>&amp;Start OCR</source>
        <translation>開始 OCR(&amp;O)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="476"/>
        <source>&amp;Settings</source>
        <translation>設定(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="481"/>
        <source>&amp;Anki Profile</source>
        <translation>Anki 設定檔(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="518"/>
        <source>&amp;Options</source>
        <translation>選項(&amp;O)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="524"/>
        <source>&amp;Open Config</source>
        <translation>開啟設定資料夾(&amp;C)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="531"/>
        <source>&amp;Check for Updates</source>
        <translation>檢查更新(&amp;U)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="536"/>
        <source>&amp;About Memento</source>
        <translation>關於 Memento(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="546"/>
        <source>Video Files (%1)</source>
        <translation>影片檔案 (%1)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="547"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="581"/>
        <source>All Files (*.*)</source>
        <translation>所有檔案 (*.*)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="558"/>
        <source>Open Stream</source>
        <translation>開啟串流</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="570"/>
        <source>Enter URL</source>
        <translation>輸入 URL</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="580"/>
        <source>Subtitle Files (%1)</source>
        <translation>字幕檔案 (%1)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="583"/>
        <source>Select Subtitle File</source>
        <translation>選擇字幕檔案</translation>
    </message>
</context>
<context>
    <name>SearchPage</name>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="64"/>
        <source>Matcher</source>
        <translation>比對方式</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="74"/>
        <source>Exact</source>
        <translation>精確</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="91"/>
        <source>Deconjugation</source>
        <translation>活用還原</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="110"/>
        <source>MeCab IPAdic</source>
        <translation>MeCab IPAdic</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="125"/>
        <source>Popup</source>
        <translation>彈出視窗</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="135"/>
        <source>Search method</source>
        <translation>搜尋方式</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="142"/>
        <source>Hover</source>
        <translation>懸停</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="146"/>
        <source>Modifier</source>
        <translation>輔助鍵</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="165"/>
        <source>Search hover millisecond delay</source>
        <translation>懸停搜尋延遲（毫秒）</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="185"/>
        <source>Search modifier key</source>
        <translation>搜尋輔助鍵</translation>
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
        <translation>使用滑鼠中鍵搜尋</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="240"/>
        <source>Glossary style</source>
        <translation>釋義樣式</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="247"/>
        <source>Bullet Points</source>
        <translation>項目符號</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="251"/>
        <source>Line Breaks</source>
        <translation>換行</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="255"/>
        <source>Pipes</source>
        <translation>直線分隔</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="274"/>
        <source>Auto play first result audio</source>
        <translation>自動播放第一個結果的音訊</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="291"/>
        <source>Show add to Anki glossary checkbox</source>
        <translation>顯示「新增至 Anki 釋義」核取方塊</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="307"/>
        <source>Subtitles</source>
        <translation>字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="317"/>
        <source>Hide mpv subtitles while Memento subtitles are visible</source>
        <translation>Memento 字幕可見時隱藏 mpv 字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="334"/>
        <source>Hide Memento subtitles while playing media</source>
        <translation>播放媒體時隱藏 Memento 字幕</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="351"/>
        <source>Pause when hovering on subtitles</source>
        <translation>懸停在字幕上時暫停</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="368"/>
        <source>Replace new lines in subtitles</source>
        <translation>取代字幕中的換行</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="385"/>
        <source>New line replacement text</source>
        <translation>換行取代文字</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="402"/>
        <source>Remove subtitle text matching regex</source>
        <translation>移除相符正規表示式的字幕文字</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="406"/>
        <source>Regex</source>
        <translation>正規表示式</translation>
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
        <translation>自動跳轉</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="256"/>
        <source>Search</source>
        <translation>搜尋</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="285"/>
        <source>No Matches</source>
        <translation>無相符項</translation>
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
        <translation>新增 Anki 筆記時發生錯誤</translation>
    </message>
    <message>
        <location filename="../qml/definition/TermEntry.qml" line="251"/>
        <source>Could Not Open Anki</source>
        <translation>無法開啟 Anki</translation>
    </message>
</context>
<context>
    <name>UpdateDialog</name>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="27"/>
        <source>Update Check Failed</source>
        <translation>更新檢查失敗</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="28"/>
        <source>&lt;p&gt;The GitHub API did not return a valid reply.&lt;/p&gt;&lt;p&gt;Check for updates manually &lt;a href=&quot;%1&quot;&gt;here&lt;/a&gt;.</source>
        <translation>&lt;p&gt;GitHub API 未傳回有效回應。&lt;/p&gt;&lt;p&gt;請&lt;a href=&quot;%1&quot;&gt;在這裡&lt;/a&gt;手動檢查更新。</translation>
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
