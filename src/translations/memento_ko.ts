<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ko_KR">
<context>
    <name>AboutWindow</name>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="8"/>
        <source>About Memento</source>
        <translation>Memento 정보</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="29"/>
        <source>Memento</source>
        <translation>Memento</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="36"/>
        <source>Version %1</source>
        <translation>버전 %1</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="44"/>
        <source>GPLv2-Only</source>
        <translation>GPLv2 전용</translation>
    </message>
    <message>
        <location filename="../qml/windows/AboutWindow.qml" line="50"/>
        <source>&lt;p&gt;&lt;a href=&quot;https://ripose-jp.github.io/Memento/&quot;&gt;Project Site&lt;/a&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;a href=&quot;https://ripose-jp.github.io/Memento/&quot;&gt;프로젝트 사이트&lt;/a&gt;&lt;/p&gt;</translation>
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
        <translation>Anki 연동 도움말</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="34"/>
        <source>Introduction</source>
        <translation>소개</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="40"/>
        <source>This document describes the syntax and the associated markers available for use in note templates.</source>
        <translation>이 문서는 노트 템플릿에서 사용할 수 있는 구문과 관련 마커를 설명합니다.</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="47"/>
        <source>Syntax</source>
        <translation>구문</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="54"/>
        <source>&lt;p&gt;A marker is a section of text enclosed in curly braces like &lt;code&gt;{subtitle}&lt;/code&gt;. When Memento&apos;s template processor sees a marker, it replaces it with its associated value. In the case of &lt;code&gt;{subtitle}&lt;/code&gt; that is the currently displayed subtitle.&lt;/p&gt;&lt;p&gt;Some markers have arguments that allow their functionality to modified. For example, &lt;code&gt;{glossary : dict = 1}&lt;/code&gt; makes it so the glossary marker only uses entries from the dictionary with the ID of 1. The &lt;code&gt;:&lt;/code&gt; character indicates the beginning of marker arguments. Arguments are set in the form of &lt;code&gt;name = value&lt;/code&gt;. If a marker has multiple arguments, arguments are separated by the &lt;code&gt;,&lt;/code&gt; character. An example of a marker with multiple arguments is &lt;code&gt;{screenshot : max-width = 1280, max-height = 720}&lt;/code&gt;.&lt;/p&gt;&lt;p&gt;A single marker can specify whether to use one or another value. For example, &lt;code&gt;{selection | clipboard}&lt;/code&gt; specifies if &lt;code&gt;selection&lt;/code&gt; is empty, use &lt;code&gt;clipboard&lt;/code&gt; instead. This functionality is triggered by the &lt;code&gt;|&lt;/code&gt; character. Multiple ors can be chained together like &lt;code&gt;{selection | clipboard | glossary : dict = 1}&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;마커는 &lt;code&gt;{subtitle}&lt;/code&gt;처럼 중괄호로 감싼 텍스트 부분입니다. Memento의 템플릿 처리기가 마커를 발견하면 연결된 값으로 바꿉니다. &lt;code&gt;{subtitle}&lt;/code&gt;의 경우 현재 표시 중인 자막입니다.&lt;/p&gt;&lt;p&gt;일부 마커에는 동작을 수정할 수 있는 인수가 있습니다. 예를 들어 &lt;code&gt;{glossary : dict = 1}&lt;/code&gt;은 뜻 마커가 ID가 1인 사전의 항목만 사용하도록 합니다. &lt;code&gt;:&lt;/code&gt; 문자는 마커 인수의 시작을 나타냅니다. 인수는 &lt;code&gt;name = value&lt;/code&gt; 형식으로 설정합니다. 마커에 여러 인수가 있으면 &lt;code&gt;,&lt;/code&gt; 문자로 구분합니다. 여러 인수가 있는 마커의 예는 &lt;code&gt;{screenshot : max-width = 1280, max-height = 720}&lt;/code&gt;입니다.&lt;/p&gt;&lt;p&gt;하나의 마커는 어떤 값이 비어 있을 때 다른 값을 사용할지 지정할 수 있습니다. 예를 들어 &lt;code&gt;{selection | clipboard}&lt;/code&gt;는 &lt;code&gt;selection&lt;/code&gt;이 비어 있으면 대신 &lt;code&gt;clipboard&lt;/code&gt;를 사용하도록 지정합니다. 이 기능은 &lt;code&gt;|&lt;/code&gt; 문자로 동작합니다. &lt;code&gt;{selection | clipboard | glossary : dict = 1}&lt;/code&gt;처럼 여러 대체 항목을 이어 붙일 수 있습니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="82"/>
        <source>Term Markers</source>
        <translation>단어 마커</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="88"/>
        <source>This section describes the markers available for use in term notes.</source>
        <translation>이 섹션은 단어 노트에서 사용할 수 있는 마커를 설명합니다.</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="111"/>
        <source>Kanji Markers</source>
        <translation>한자 마커</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationHelpWindow.qml" line="117"/>
        <source>This section describes the markers available for use in kanji notes.</source>
        <translation>이 섹션은 한자 노트에서 사용할 수 있는 마커를 설명합니다.</translation>
    </message>
</context>
<context>
    <name>AnkiIntegrationPage</name>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="89"/>
        <source>Add Profile</source>
        <translation>프로필 추가</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="95"/>
        <source>Profile name</source>
        <translation>프로필 이름</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="101"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="106"/>
        <source>Copy current profile</source>
        <translation>현재 프로필 복사</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="206"/>
        <source>General</source>
        <translation>일반</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="216"/>
        <source>Enable Anki integration</source>
        <translation>Anki 연동 활성화</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="233"/>
        <source>Show advanced settings</source>
        <translation>고급 설정 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="247"/>
        <source>Profile</source>
        <translation>프로필</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="257"/>
        <source>Current profile</source>
        <translation>현재 프로필</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="304"/>
        <source>A profile with this name already exists</source>
        <translation>이 이름의 프로필이 이미 있습니다</translation>
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
        <translation>호스트 이름</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="353"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="357"/>
        <source>Port</source>
        <translation>포트</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="375"/>
        <source>Use API key</source>
        <translation>API 키 사용</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="395"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="399"/>
        <source>API key</source>
        <translation>API 키</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="413"/>
        <source>Test connection</source>
        <translation>연결 테스트</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="417"/>
        <source>Connect</source>
        <translation>연결</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="424"/>
        <source>Anki Connection</source>
        <translation>Anki 연결</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="425"/>
        <source>Connection successful.</source>
        <translation>연결되었습니다.</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="430"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="714"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="751"/>
        <source>Anki Connection Error</source>
        <translation>Anki 연결 오류</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="431"/>
        <source>Failed to connect to Anki: %1.</source>
        <translation>Anki에 연결하지 못했습니다: %1.</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="447"/>
        <source>Advanced</source>
        <translation>고급</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="457"/>
        <source>Duplicate policy</source>
        <translation>중복 처리 방식</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="464"/>
        <source>None</source>
        <translation>없음</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="468"/>
        <source>Allowed in different decks</source>
        <translation>다른 덱에서는 허용</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="472"/>
        <source>Allowed in same deck</source>
        <translation>같은 덱에서도 허용</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="491"/>
        <source>Replace newlines with</source>
        <translation>줄바꿈을 다음으로 바꾸기</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="495"/>
        <source>Replacement string</source>
        <translation>대체 문자열</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="509"/>
        <source>Screenshot file type</source>
        <translation>스크린샷 파일 형식</translation>
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
        <translation>오디오 시작 패딩 길이</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="548"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="576"/>
        <source>Seconds</source>
        <translation>초</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="571"/>
        <source>Audio padding end duration</source>
        <translation>오디오 끝 패딩 길이</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="599"/>
        <source>Audio media normalization</source>
        <translation>오디오 미디어 정규화</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="616"/>
        <source>Audio normalization dB</source>
        <translation>오디오 정규화 dB</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="621"/>
        <source>dB</source>
        <translation>dB</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="642"/>
        <source>Include in glossary</source>
        <translation>뜻에 포함</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="669"/>
        <source>Cards</source>
        <translation>카드</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="679"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="684"/>
        <source>Tags</source>
        <translation>태그</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="696"/>
        <source>Term Notes</source>
        <translation>단어 노트</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="715"/>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="752"/>
        <source>Could not get model fields: %1.</source>
        <translation>모델 필드를 가져올 수 없습니다: %1.</translation>
    </message>
    <message>
        <location filename="../qml/options/AnkiIntegrationPage.qml" line="733"/>
        <source>Kanji Notes</source>
        <translation>한자 노트</translation>
    </message>
</context>
<context>
    <name>AnkiMarkerDescription</name>
    <message>
        <location filename="../qml/controls/AnkiMarkerDescription.qml" line="52"/>
        <source>&lt;p&gt;&lt;b&gt;Default Value:&lt;/b&gt; %1&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;기본값:&lt;/b&gt; %1&lt;/p&gt;</translation>
    </message>
</context>
<context>
    <name>AnkiMarkers</name>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="48"/>
        <source>&lt;p&gt;Audio of the subtitle list selection.&lt;/p&gt;</source>
        <translation>&lt;p&gt;자막 목록 선택 항목의 오디오입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="51"/>
        <source>&lt;p&gt;Audio of the word if found and the skip hash is not matched.&lt;/p&gt;</source>
        <translation>&lt;p&gt;단어를 찾았고 건너뛰기 해시가 일치하지 않을 때의 단어 오디오입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="54"/>
        <source>&lt;p&gt;Audio of the current subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;현재 자막의 오디오입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="57"/>
        <source>&lt;p&gt;The kanji character.&lt;/p&gt;</source>
        <translation>&lt;p&gt;한자 문자입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="60"/>
        <source>&lt;p&gt;The current text in the clipboard.&lt;/p&gt;</source>
        <translation>&lt;p&gt;클립보드의 현재 텍스트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="63"/>
        <source>&lt;p&gt;The raw expression as recognized by Memento.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Memento가 인식한 원본 표현입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="66"/>
        <source>&lt;p&gt;The text before &lt;code&gt;{%1}&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;code&gt;{%1}&lt;/code&gt; 앞의 텍스트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="69"/>
        <source>&lt;p&gt;The text after &lt;code&gt;{%1}&lt;/code&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;code&gt;{%1}&lt;/code&gt; 뒤의 텍스트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="72"/>
        <source>&lt;p&gt;Text of all selected lines in the secondary subtitle list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;보조 자막 목록에서 선택한 모든 줄의 텍스트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="75"/>
        <source>&lt;p&gt;Text of all selected lines in the primary subtitle list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;기본 자막 목록에서 선택한 모든 줄의 텍스트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="78"/>
        <source>&lt;p&gt;The expression in kanji. Kana if there is no kanji or explicitly requested.&lt;/p&gt;</source>
        <translation>&lt;p&gt;한자로 된 표현입니다. 한자가 없거나 명시적으로 요청한 경우 가나입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="81"/>
        <source>&lt;p&gt;The average of frequency data for the expression. Defaults to 0 occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현의 빈도 데이터 평균입니다. 빈도 데이터가 없으면 발생 횟수는 기본값 0입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="85"/>
        <source>&lt;p&gt;The average of frequency data for the expression. Defaults to 9999999 when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현의 빈도 데이터 평균입니다. 빈도 데이터가 없으면 기본값은 9999999입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="89"/>
        <source>&lt;p&gt;The harmonic mean of frequency data for the expression. Defaults to 0 for occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현의 빈도 데이터 조화 평균입니다. 빈도 데이터가 없으면 발생 횟수는 기본값 0입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="93"/>
        <source>&lt;p&gt;The harmonic mean of frequency data for the expression. Defaults to 9999999 for occurrences when frequency data is not available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현의 빈도 데이터 조화 평균입니다. 빈도 데이터가 없으면 발생 횟수는 기본값 9999999입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="97"/>
        <source>&lt;p&gt;Bulleted list of frequency tags.&lt;/p&gt;</source>
        <translation>&lt;p&gt;빈도 태그의 글머리 기호 목록입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="100"/>
        <source>&lt;p&gt;The expression in kanji with furigana above. Kana if there is no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;후리가나가 위에 표시된 한자 표현입니다. 한자가 없으면 가나입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="104"/>
        <source>&lt;p&gt;The expression in kanji with furigana proceeding in brackets. Kana if there is no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;후리가나가 괄호 안에 뒤따르는 한자 표현입니다. 한자가 없으면 가나입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="108"/>
        <source>&lt;p&gt;Dictionary definitions of the expression without dictionary information.&lt;/p&gt;</source>
        <translation>&lt;p&gt;사전 정보가 없는 표현의 사전 정의입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="111"/>
        <source>&lt;p&gt;Dictionary definitions of the expression in compact form.&lt;/p&gt;</source>
        <translation>&lt;p&gt;간략한 형식의 표현 사전 정의입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="114"/>
        <source>&lt;p&gt;Dictionary definitions of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현의 사전 정의입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="117"/>
        <source>&lt;p&gt;A comma separated list of kunyomi (Japanese) readings written in katakana.&lt;/p&gt;</source>
        <translation>&lt;p&gt;가타카나로 쓰인 훈독(일본식 읽기)의 쉼표 구분 목록입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="120"/>
        <source>&lt;p&gt;A comma separated list of onyomi (Chinese) readings written in hiragana.&lt;/p&gt;</source>
        <translation>&lt;p&gt;히라가나로 쓰인 음독(중국식 읽기)의 쉼표 구분 목록입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="123"/>
        <source>&lt;p&gt;A comma separated list of pitch accent categories for the expression: heiban, kifuku, atamadaka, nakadaka, odaka.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현의 피치 악센트 분류 쉼표 구분 목록입니다: heiban, kifuku, atamadaka, nakadaka, odaka.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="127"/>
        <source>&lt;p&gt;Pitch accents for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현 읽기의 피치 악센트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="130"/>
        <source>&lt;p&gt;Pitch graphs for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현 읽기의 피치 그래프입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="133"/>
        <source>&lt;p&gt;Pitch positions for the reading of the expression.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현 읽기의 피치 위치입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="136"/>
        <source>&lt;p&gt;The reading of the expression in kana. Empty if the expression has no kanji.&lt;/p&gt;</source>
        <translation>&lt;p&gt;가나로 된 표현의 읽기입니다. 표현에 한자가 없으면 비어 있습니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="140"/>
        <source>&lt;p&gt;Screenshot of the current frame.&lt;/p&gt;</source>
        <translation>&lt;p&gt;현재 프레임의 스크린샷입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="143"/>
        <source>&lt;p&gt;Screenshot of the current frame without subtitles if visible.&lt;/p&gt;</source>
        <translation>&lt;p&gt;자막이 표시 중이면 자막을 제외한 현재 프레임의 스크린샷입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="146"/>
        <source>&lt;p&gt;The currently selected glossary text. If multiple glossaries are selected, they are ordered as a list.&lt;/p&gt;</source>
        <translation>&lt;p&gt;현재 선택한 뜻입니다. 여러 뜻을 선택한 경우 목록으로 정렬됩니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="150"/>
        <source>&lt;p&gt;The number of strokes in the character.&lt;/p&gt;</source>
        <translation>&lt;p&gt;문자의 획수입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="153"/>
        <source>&lt;p&gt;The text of the secondary subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;보조 자막의 텍스트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="156"/>
        <source>&lt;p&gt;The text of the primary subtitle.&lt;/p&gt;</source>
        <translation>&lt;p&gt;기본 자막의 텍스트입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="159"/>
        <source>&lt;p&gt;Unordered list of expression tags without descriptions.&lt;/p&gt;</source>
        <translation>&lt;p&gt;설명 없는 표현 태그의 순서 없는 목록입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="162"/>
        <source>&lt;p&gt;Unordered list of expression tags.&lt;/p&gt;</source>
        <translation>&lt;p&gt;표현 태그의 순서 없는 목록입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="165"/>
        <source>&lt;p&gt;Title of the current media. Filename if no title is set.&lt;/p&gt;</source>
        <translation>&lt;p&gt;현재 미디어의 제목입니다. 제목이 없으면 파일 이름입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="168"/>
        <source>&lt;p&gt;An MP4 file containing a clip of the current context.&lt;/p&gt;</source>
        <translation>&lt;p&gt;현재 컨텍스트의 클립을 포함하는 MP4 파일입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="184"/>
        <source>&lt;p&gt;If true, audio is included in the clip.&lt;/p&gt;</source>
        <translation>&lt;p&gt;true이면 클립에 오디오를 포함합니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="187"/>
        <source>&lt;p&gt;The decibel value to normalize audio to. Ignored if &lt;code&gt;%1 = false&lt;/code&gt;.&lt;/p&gt;</source>
        <translation>&lt;p&gt;오디오를 정규화할 데시벨 값입니다. &lt;code&gt;%1 = false&lt;/code&gt;이면 무시됩니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="191"/>
        <source>&lt;p&gt;Filter to only use entries from a particular dictionary. Takes a dictionary ID. Dictionary IDs can be found in &lt;b&gt;Options&lt;/b&gt; under &lt;b&gt;Dictionaries&lt;/b&gt; at the end of dictionary names.&lt;/p&gt;</source>
        <translation>&lt;p&gt;특정 사전의 항목만 사용하도록 필터링합니다. 사전 ID를 받습니다. 사전 ID는 &lt;b&gt;옵션&lt;/b&gt;의 &lt;b&gt;사전&lt;/b&gt;에서 사전 이름 끝에서 확인할 수 있습니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="196"/>
        <source>&lt;p&gt;If true, maintains the aspect ratio of the image during resize.&lt;/p&gt;</source>
        <translation>&lt;p&gt;true이면 크기 조정 중 이미지의 가로세로 비율을 유지합니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="199"/>
        <source>&lt;p&gt;Used to determine the maximum height of a saved screenshot. A value of -1 leaves this unbounded.&lt;/p&gt;</source>
        <translation>&lt;p&gt;저장된 스크린샷의 최대 높이를 결정하는 데 사용됩니다. 값이 -1이면 제한하지 않습니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="203"/>
        <source>&lt;p&gt;Used to determine the maximum width of a saved screenshot. A value of -1 leaves this unbounded.&lt;/p&gt;</source>
        <translation>&lt;p&gt;저장된 스크린샷의 최대 너비를 결정하는 데 사용됩니다. 값이 -1이면 제한하지 않습니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="207"/>
        <source>&lt;p&gt;If true, only export the lowest frequency value.&lt;/p&gt;</source>
        <translation>&lt;p&gt;true이면 가장 낮은 빈도 값만 내보냅니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="210"/>
        <source>&lt;p&gt;If true, normalize audio.&lt;/p&gt;</source>
        <translation>&lt;p&gt;true이면 오디오를 정규화합니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="213"/>
        <source>&lt;p&gt;The source of clip timings. Possible values are:&lt;ul&gt;&lt;li&gt;&lt;b&gt;subtitle&lt;/b&gt;: Use timings from the current subtitle.&lt;/li&gt;&lt;li&gt;&lt;b&gt;context&lt;/b&gt;: Use timings from the currently selected context.&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;클립 시간의 소스입니다. 가능한 값은 다음과 같습니다:&lt;ul&gt;&lt;li&gt;&lt;b&gt;subtitle&lt;/b&gt;: 현재 자막의 시간을 사용합니다.&lt;/li&gt;&lt;li&gt;&lt;b&gt;context&lt;/b&gt;: 현재 선택한 문맥의 시간을 사용합니다.&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="222"/>
        <source>&lt;p&gt;If true, subtitles are included in the clip.&lt;/p&gt;</source>
        <translation>&lt;p&gt;true이면 클립에 자막을 포함합니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="225"/>
        <source>&lt;p&gt;If true, export frequency values without dictionary names.&lt;/p&gt;</source>
        <translation>&lt;p&gt;true이면 사전 이름 없이 빈도 값을 내보냅니다.&lt;/p&gt;</translation>
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
        <translation>프로필 정규화 dB 설정</translation>
    </message>
    <message>
        <location filename="../qml/util/AnkiMarkers.qml" line="231"/>
        <source>None</source>
        <translation>없음</translation>
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
        <translation>프로필 오디오 미디어 정규화 설정</translation>
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
        <translation>덱</translation>
    </message>
    <message>
        <location filename="../qml/controls/AnkiNoteBox.qml" line="52"/>
        <source>Model</source>
        <translation>모델</translation>
    </message>
</context>
<context>
    <name>ApplicationPage</name>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="66"/>
        <source>Language</source>
        <translation>언어</translation>
    </message>
    <message>
        <source>Interface language</source>
        <translation type="vanished">인터페이스 언어</translation>
    </message>
    <message>
        <source>Interface language (requires restart)</source>
        <translation type="vanished">인터페이스 언어 (재시작 필요)</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="76"/>
        <source>Interface language (Restart required)</source>
        <translation>인터페이스 언어 (재시작 필요)</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="83"/>
        <source>System</source>
        <translation>시스템</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="118"/>
        <source>Update</source>
        <translation>업데이트</translation>
    </message>
    <message>
        <location filename="../qml/options/ApplicationPage.qml" line="128"/>
        <source>Check for updates on launch</source>
        <translation>실행 시 업데이트 확인</translation>
    </message>
</context>
<context>
    <name>AudioSourcesPage</name>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="65"/>
        <source>Audio Sources Help</source>
        <translation>오디오 소스 도움말</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="70"/>
        <source>&lt;p&gt;&lt;b&gt;Source Name&lt;/b&gt;: The name of the audio source as it will appear in Memento.&lt;/p&gt;
                        &lt;p&gt;&lt;b&gt;URL&lt;/b&gt;: The URL of the audio source. Supports inserting &lt;b&gt;{expression}&lt;/b&gt; and
                            &lt;b&gt;{reading}&lt;/b&gt; markers into the URL.&lt;/p&gt;
                        &lt;p&gt;&lt;b&gt;MD5 Skip Hash&lt;/b&gt;: Audio that matches this MD5 hash will be ignored.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;소스 이름&lt;/b&gt;: Memento에 표시될 오디오 소스의 이름입니다.&lt;/p&gt;&lt;p&gt;&lt;b&gt;URL&lt;/b&gt;: 오디오 소스의 URL입니다. URL에 &lt;b&gt;{expression}&lt;/b&gt; 및 &lt;b&gt;{reading}&lt;/b&gt; 마커를 삽입할 수 있습니다.&lt;/p&gt;&lt;p&gt;&lt;b&gt;MD5 건너뛰기 해시&lt;/b&gt;: 이 MD5 해시와 일치하는 오디오는 무시됩니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="112"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="119"/>
        <source>URL</source>
        <translation>URL</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="128"/>
        <source>File</source>
        <translation>파일</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="132"/>
        <source>JSON</source>
        <translation>JSON</translation>
    </message>
    <message>
        <location filename="../qml/options/AudioSourcesPage.qml" line="143"/>
        <source>MD5 Skip Hash</source>
        <translation>MD5 건너뛰기 해시</translation>
    </message>
</context>
<context>
    <name>BehaviorPage</name>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="15"/>
        <source>Select Custom Open Folder</source>
        <translation>사용자 지정 열기 폴더 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="72"/>
        <source>Player</source>
        <translation>플레이어</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="82"/>
        <source>Auto fit media on load</source>
        <translation>미디어 로드 시 자동 맞춤</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="99"/>
        <source>Auto fit media percentage</source>
        <translation>미디어 자동 맞춤 비율</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="117"/>
        <source>On Screen Controller</source>
        <translation>화면 컨트롤러</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="127"/>
        <source>Visibility duration milliseconds</source>
        <translation>표시 지속 시간(밀리초)</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="147"/>
        <source>Fade duration milliseconds</source>
        <translation>페이드 지속 시간(밀리초)</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="167"/>
        <source>Minimum mouse pixel movement</source>
        <translation>최소 마우스 이동 픽셀</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="187"/>
        <source>Allow mpv to control cursor visibility</source>
        <translation>mpv가 커서 표시를 제어하도록 허용</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="204"/>
        <source>Show preview thumbnails</source>
        <translation>미리보기 썸네일 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="219"/>
        <source>Subtitles</source>
        <translation>자막</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="229"/>
        <source>Automatically pause at the end of subtitles</source>
        <translation>자막 끝에서 자동 일시정지</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="246"/>
        <source>Show subtitles when the cursor is at the bottom of the player</source>
        <translation>커서가 플레이어 하단에 있을 때 자막 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="263"/>
        <source>Show secondary subtitles when the cursor is at the top of the player</source>
        <translation>커서가 플레이어 상단에 있을 때 보조 자막 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="279"/>
        <source>Open File Directory</source>
        <translation>파일 열기 디렉터리</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="289"/>
        <source>Open file starting location</source>
        <translation>파일 열기 시작 위치</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="296"/>
        <source>Current</source>
        <translation>현재</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="300"/>
        <source>Custom</source>
        <translation>사용자 지정</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="304"/>
        <source>Documents</source>
        <translation>문서</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="308"/>
        <source>Home</source>
        <translation>홈</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="312"/>
        <source>Movies</source>
        <translation>동영상</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="331"/>
        <source>Custom file directory</source>
        <translation>사용자 지정 파일 디렉터리</translation>
    </message>
    <message>
        <location filename="../qml/options/BehaviorPage.qml" line="335"/>
        <source>Browse</source>
        <translation>찾아보기</translation>
    </message>
</context>
<context>
    <name>DatabaseManager</name>
    <message>
        <location filename="../dict/databasemanager.cpp" line="395"/>
        <location filename="../dict/databasemanager.cpp" line="526"/>
        <source>Could not prepare database query</source>
        <translation>데이터베이스 쿼리를 준비할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="404"/>
        <location filename="../dict/databasemanager.cpp" line="416"/>
        <location filename="../dict/databasemanager.cpp" line="428"/>
        <location filename="../dict/databasemanager.cpp" line="534"/>
        <source>Could not bind values to statement</source>
        <translation>SQL 문에 값을 바인딩할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="463"/>
        <source>Error when executing sqlite query. Code %1</source>
        <translation>SQLite 쿼리 실행 중 오류가 발생했습니다. 코드 %1</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="473"/>
        <source>Error getting term information</source>
        <translation>단어 정보를 가져오는 중 오류가 발생했습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="608"/>
        <source>Error while executing kanji query</source>
        <translation>한자 쿼리 실행 중 오류가 발생했습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1105"/>
        <source>Could not add index.json</source>
        <translation>index.json을 추가할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1107"/>
        <source>Could not add kanji terms</source>
        <translation>한자 단어를 추가할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1109"/>
        <source>Could not add kanji metadata</source>
        <translation>한자 메타데이터를 추가할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1111"/>
        <source>Could not add tags</source>
        <translation>태그를 추가할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1113"/>
        <source>Could not add terms</source>
        <translation>단어를 추가할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1115"/>
        <source>Could not add term metadata</source>
        <translation>단어 메타데이터를 추가할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1117"/>
        <source>Could not open database</source>
        <translation>데이터베이스를 열 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1119"/>
        <source>Database is of a newer version</source>
        <translation>데이터베이스가 더 최신 버전입니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1121"/>
        <source>Could not open dictionary file</source>
        <translation>사전 파일을 열 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1123"/>
        <source>Could not execute delete query on database</source>
        <translation>데이터베이스에서 삭제 쿼리를 실행할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1125"/>
        <source>Could not extract dictionary resources</source>
        <translation>사전 리소스를 추출할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1127"/>
        <source>Could not remove dictionary resources</source>
        <translation>사전 리소스를 제거할 수 없습니다</translation>
    </message>
    <message>
        <location filename="../dict/databasemanager.cpp" line="1129"/>
        <source>Unknown error</source>
        <translation>알 수 없는 오류</translation>
    </message>
</context>
<context>
    <name>Deconjugator</name>
    <message>
        <location filename="../dict/deconjugator.cpp" line="475"/>
        <source>godan verb</source>
        <translation>5단 동사</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="477"/>
        <source>ichidan verb</source>
        <translation>1단 동사</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="479"/>
        <source>suru verb</source>
        <translation>する 동사</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="481"/>
        <source>kuru verb</source>
        <translation>くる 동사</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="483"/>
        <source>adjective</source>
        <translation>형용사</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="485"/>
        <source>negative</source>
        <translation>부정</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="487"/>
        <source>past</source>
        <translation>과거</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="489"/>
        <source>-te</source>
        <translation>て형</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="491"/>
        <source>masu stem</source>
        <translation>ます 어간</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="493"/>
        <source>volitional</source>
        <translation>의지형</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="495"/>
        <source>passive</source>
        <translation>수동</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="497"/>
        <source>causative</source>
        <translation>사역</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="499"/>
        <source>imperative</source>
        <translation>명령</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="501"/>
        <source>potential</source>
        <translation>가능</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="503"/>
        <source>potential or passive</source>
        <translation>가능 또는 수동</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="505"/>
        <source>imperative negative</source>
        <translation>부정 명령</translation>
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
        <translation>정중</translation>
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
        <translation>구어체 부정</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="545"/>
        <source>provisional colloquial negative</source>
        <translation>조건형 구어체 부정</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="547"/>
        <source>progressive or perfect</source>
        <translation>진행 또는 완료</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="549"/>
        <source>adv</source>
        <translation>부사</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="551"/>
        <source>noun</source>
        <translation>명사</translation>
    </message>
    <message>
        <location filename="../dict/deconjugator.cpp" line="553"/>
        <source>unknown</source>
        <translation>알 수 없음</translation>
    </message>
</context>
<context>
    <name>DictionaryPage</name>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="17"/>
        <source>Dictionary Files (*.zip)</source>
        <translation>사전 파일 (*.zip)</translation>
    </message>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="18"/>
        <source>Select Dictionaries</source>
        <translation>사전 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/DictionaryPage.qml" line="33"/>
        <source>Dictionary Error</source>
        <translation>사전 오류</translation>
    </message>
</context>
<context>
    <name>InterfacePage</name>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="66"/>
        <source>Theme</source>
        <translation>테마</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="76"/>
        <source>Use system icons</source>
        <translation>시스템 아이콘 사용</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="92"/>
        <source>Subtitle</source>
        <translation>자막</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="102"/>
        <source>Font</source>
        <translation>글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="111"/>
        <source>Select Subtitle Font</source>
        <translation>자막 글꼴 선택</translation>
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
        <translation>굵게</translation>
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
        <translation>기울임꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="167"/>
        <source>Line spacing</source>
        <translation>줄 간격</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="173"/>
        <location filename="../qml/options/InterfacePage.qml" line="242"/>
        <source>Pixels</source>
        <translation>픽셀</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="187"/>
        <source>Line height as a percentage of window height</source>
        <translation>창 높이 대비 줄 높이 비율</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="196"/>
        <location filename="../qml/options/InterfacePage.qml" line="219"/>
        <source>Percent</source>
        <translation>퍼센트</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="210"/>
        <source>Bottom offset as a percentage of window height</source>
        <translation>창 높이 대비 하단 오프셋 비율</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="233"/>
        <source>Stroke size</source>
        <translation>테두리 크기</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="256"/>
        <location filename="../qml/options/InterfacePage.qml" line="1018"/>
        <source>Text color</source>
        <translation>텍스트 색상</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="269"/>
        <source>Select Subtitle Text Color</source>
        <translation>자막 텍스트 색상 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="284"/>
        <location filename="../qml/options/InterfacePage.qml" line="990"/>
        <source>Background color</source>
        <translation>배경색</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="297"/>
        <source>Select Subtitle Background Color</source>
        <translation>자막 배경색 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="312"/>
        <source>Stroke color</source>
        <translation>테두리 색상</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="325"/>
        <source>Select Subtitle Stroke Color</source>
        <translation>자막 테두리 색상 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="370"/>
        <source>Search</source>
        <translation>검색</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="380"/>
        <source>Term Expression Font</source>
        <translation>단어 표기 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="389"/>
        <source>Select Term Expression Font</source>
        <translation>단어 표기 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="445"/>
        <source>Term Reading Font</source>
        <translation>단어 읽기 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="454"/>
        <source>Select Term Reading Font</source>
        <translation>단어 읽기 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="510"/>
        <source>Term Conjugation Explanation Font</source>
        <translation>단어 활용 설명 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="519"/>
        <source>Select Term Conjugation Explanation Font</source>
        <translation>단어 활용 설명 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="575"/>
        <source>Tag Font</source>
        <translation>태그 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="584"/>
        <source>Select Tag Font</source>
        <translation>태그 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="640"/>
        <source>Glossary Font</source>
        <translation>뜻 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="649"/>
        <source>Select Glossary Font</source>
        <translation>뜻 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="705"/>
        <source>Kanji Font</source>
        <translation>한자 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="714"/>
        <source>Select Kanji Font</source>
        <translation>한자 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="768"/>
        <source>Popup</source>
        <translation>팝업</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="778"/>
        <source>Popup width</source>
        <translation>팝업 너비</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="798"/>
        <source>Popup height</source>
        <translation>팝업 높이</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="816"/>
        <source>Subtitle List</source>
        <translation>자막 목록</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="826"/>
        <location filename="../qml/options/InterfacePage.qml" line="1139"/>
        <source>Open in a separate window</source>
        <translation>별도 창에서 열기</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="843"/>
        <source>Show timestamps</source>
        <translation>타임스탬프 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="860"/>
        <source>Primary subtitle font</source>
        <translation>기본 자막 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="869"/>
        <source>Select Subtitle List Primary Font</source>
        <translation>자막 목록 기본 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="925"/>
        <source>Secondary subtitle font</source>
        <translation>보조 자막 글꼴</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="934"/>
        <source>Select Subtitle List Secondary Font</source>
        <translation>자막 목록 보조 글꼴 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1003"/>
        <source>Select Subtitle List Background Color</source>
        <translation>자막 목록 배경색 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1031"/>
        <source>Select Subtitle List Text Color</source>
        <translation>자막 목록 텍스트 색상 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1046"/>
        <source>Hover color</source>
        <translation>호버 색상</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1059"/>
        <source>Select Subtitle List Hover Color</source>
        <translation>자막 목록 호버 색상 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1074"/>
        <source>Selection color</source>
        <translation>선택 색상</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1087"/>
        <source>Select Subtitle List Selection Color</source>
        <translation>자막 목록 선택 색상 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1102"/>
        <source>Hovered selection color</source>
        <translation>호버된 선택 색상</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1115"/>
        <source>Select Subtitle List Hovered Selection Color</source>
        <translation>자막 목록 호버된 선택 색상 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/InterfacePage.qml" line="1129"/>
        <source>Auxiliary Search</source>
        <translation>보조 검색</translation>
    </message>
</context>
<context>
    <name>KanjiEntry</name>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="55"/>
        <source>Error Adding Anki Note</source>
        <translation>Anki 노트 추가 오류</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="151"/>
        <source>Could Not Open Anki</source>
        <translation>Anki를 열 수 없습니다</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="230"/>
        <source>Glossary</source>
        <translation>뜻</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="237"/>
        <source>Reading</source>
        <translation>읽기</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="244"/>
        <source>Statistics</source>
        <translation>통계</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="312"/>
        <source>Classifications</source>
        <translation>분류</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="341"/>
        <source>Codepoints</source>
        <translation>코드 포인트</translation>
    </message>
    <message>
        <location filename="../qml/definition/KanjiEntry.qml" line="370"/>
        <source>Dictionary Indices</source>
        <translation>사전 색인</translation>
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
        <translation>할당 안 됨</translation>
    </message>
    <message>
        <location filename="../qml/controls/KeybindCommand.qml" line="93"/>
        <source>Clear</source>
        <translation>지우기</translation>
    </message>
</context>
<context>
    <name>KeybindsPage</name>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="53"/>
        <source>Add Profile</source>
        <translation>프로필 추가</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="59"/>
        <source>Profile name</source>
        <translation>프로필 이름</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="65"/>
        <source>Name</source>
        <translation>이름</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="70"/>
        <source>Copy current profile</source>
        <translation>현재 프로필 복사</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="178"/>
        <source>Add current expression to Anki</source>
        <translation>현재 표현을 Anki에 추가</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="193"/>
        <source>Scroll to previous search result</source>
        <translation>이전 검색 결과로 스크롤</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="208"/>
        <source>Scroll to next search result</source>
        <translation>다음 검색 결과로 스크롤</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="223"/>
        <source>Open file</source>
        <translation>파일 열기</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="238"/>
        <source>Open URL</source>
        <translation>URL 열기</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="253"/>
        <source>Cycle OSC visibility</source>
        <translation>OSC 표시 상태 전환</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="268"/>
        <source>Show auxiliary search</source>
        <translation>보조 검색 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="283"/>
        <source>Show subtitle list</source>
        <translation>자막 목록 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="300"/>
        <source>Start OCR</source>
        <translation>OCR 시작</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="315"/>
        <source>Toggle subtitle auto pause</source>
        <translation>자막 자동 일시정지 전환</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="330"/>
        <source>Toggle subtitle visibility</source>
        <translation>자막 표시 전환</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="345"/>
        <source>Decrease subtitle size</source>
        <translation>자막 크기 줄이기</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="360"/>
        <source>Increase subtitle size</source>
        <translation>자막 크기 늘리기</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="375"/>
        <source>Move subtitle up</source>
        <translation>자막 위로 이동</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="390"/>
        <source>Move subtitle down</source>
        <translation>자막 아래로 이동</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="405"/>
        <source>Copy subtitle list selection</source>
        <translation>자막 목록 선택 항목 복사</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="420"/>
        <source>Subtitle list find</source>
        <translation>자막 목록 찾기</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="435"/>
        <source>Subtitle list find next</source>
        <translation>자막 목록 다음 찾기</translation>
    </message>
    <message>
        <location filename="../qml/options/KeybindsPage.qml" line="450"/>
        <source>Subtitle list find previous</source>
        <translation>자막 목록 이전 찾기</translation>
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
        <translation>검색</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="287"/>
        <source>Subtitle List</source>
        <translation>자막 목록</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="327"/>
        <source>No Dictionaries Installed</source>
        <translation>설치된 사전 없음</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="333"/>
        <source>&lt;p&gt;For searching to work, please install a dictionary.&lt;/p&gt;&lt;p&gt;Dictionaries can be found &lt;a href=&apos;https://yomitan.wiki/dictionaries/&apos;&gt;here&lt;/a&gt;.&lt;/p&gt;&lt;p&gt;To install a dictionary, go to %1.&lt;/p&gt;</source>
        <translation>&lt;p&gt;검색을 사용하려면 사전을 설치하세요.&lt;/p&gt;&lt;p&gt;사전은 &lt;a href=&apos;https://yomitan.wiki/dictionaries/&apos;&gt;여기&lt;/a&gt;에서 찾을 수 있습니다.&lt;/p&gt;&lt;p&gt;사전을 설치하려면 %1(으)로 이동하세요.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="338"/>
        <source>Memento → Preferences → Dictionary</source>
        <translation>Memento → 환경설정 → 사전</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="339"/>
        <source>Settings → Options → Dictionary</source>
        <translation>설정 → 옵션 → 사전</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="360"/>
        <source>Auto Update</source>
        <translation>자동 업데이트</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="363"/>
        <source>Do you want to check for updates on launch?

This can be disabled at any time by going to %1.
This will check GitHub&apos;s API when Memento is launched.</source>
        <translation>실행 시 업데이트를 확인하시겠습니까?

이 기능은 언제든지 %1에서 비활성화할 수 있습니다.
Memento가 실행될 때 GitHub API를 확인합니다.</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="368"/>
        <source>Memento → Preferences → Application</source>
        <translation>Memento → 환경설정 → 애플리케이션</translation>
    </message>
    <message>
        <location filename="../qml/Main.qml" line="369"/>
        <source>Settings → Options → Application</source>
        <translation>설정 → 옵션 → 애플리케이션</translation>
    </message>
</context>
<context>
    <name>ManualSearchPage</name>
    <message>
        <location filename="../qml/controls/ManualSearchPage.qml" line="31"/>
        <source>Search</source>
        <translation>검색</translation>
    </message>
</context>
<context>
    <name>OcrPage</name>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="15"/>
        <source>Select Custom Open Folder</source>
        <translation>사용자 지정 열기 폴더 선택</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="73"/>
        <source>OCR</source>
        <translation>OCR</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="83"/>
        <source>Enabled</source>
        <translation>활성화</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="100"/>
        <source>Use GPU acceleration</source>
        <translation>GPU 가속 사용</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="117"/>
        <source>Use model</source>
        <translation>모델 사용</translation>
    </message>
    <message>
        <location filename="../qml/options/OcrPage.qml" line="122"/>
        <source>Hugging Face Model</source>
        <translation>Hugging Face 모델</translation>
    </message>
</context>
<context>
    <name>OptionsWindow</name>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="12"/>
        <source>Options</source>
        <translation>옵션</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="25"/>
        <source>Anki Integration</source>
        <translation>Anki 연동</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="30"/>
        <source>Application</source>
        <translation>애플리케이션</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="35"/>
        <source>Audio Sources</source>
        <translation>오디오 소스</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="40"/>
        <source>Behavior</source>
        <translation>동작</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="45"/>
        <source>Dictionary</source>
        <translation>사전</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="50"/>
        <source>Interface</source>
        <translation>인터페이스</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="55"/>
        <source>Keybinds</source>
        <translation>키 바인딩</translation>
    </message>
    <message>
        <location filename="../qml/options/OptionsWindow.qml" line="60"/>
        <source>Search</source>
        <translation>검색</translation>
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
        <translation>OSC 표시 상태: %1</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="148"/>
        <source>Visible</source>
        <translation>표시</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="153"/>
        <source>Hidden</source>
        <translation>숨김</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="158"/>
        <source>Auto</source>
        <translation>자동</translation>
    </message>
    <message>
        <location filename="../qml/controls/Player.qml" line="261"/>
        <source>Playing media</source>
        <translation>미디어 재생 중</translation>
    </message>
</context>
<context>
    <name>PlayerMenu</name>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="43"/>
        <source>Track %1</source>
        <translation>트랙 %1</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="73"/>
        <source>&amp;Media</source>
        <translation>미디어(&amp;M)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="77"/>
        <source>&amp;Open File...</source>
        <translation>파일 열기(&amp;O)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="91"/>
        <source>&amp;Open URL...</source>
        <translation>URL 열기(&amp;U)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="106"/>
        <source>&amp;Recent</source>
        <translation>최근 항목(&amp;R)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="136"/>
        <source>&amp;Clear Recents</source>
        <translation>최근 항목 지우기(&amp;C)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="144"/>
        <source>&amp;Audio</source>
        <translation>오디오(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="156"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="313"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="370"/>
        <source>None</source>
        <translation>없음</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="190"/>
        <source>&amp;Subtitle</source>
        <translation>자막(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="196"/>
        <source>&amp;Auto Pause</source>
        <translation>자동 일시정지(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="210"/>
        <source>&amp;Size and Position</source>
        <translation>크기 및 위치(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="214"/>
        <source>&amp;Increase Size</source>
        <translation>크기 늘리기(&amp;I)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="230"/>
        <source>&amp;Decrease Size</source>
        <translation>크기 줄이기(&amp;D)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="246"/>
        <source>&amp;Move Up</source>
        <translation>위로 이동(&amp;U)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="262"/>
        <source>&amp;Move Down</source>
        <translation>아래로 이동(&amp;D)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="279"/>
        <source>&amp;Show Subtitles</source>
        <translation>자막 표시(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="296"/>
        <source>&amp;Add Subtitle...</source>
        <translation>자막 추가(&amp;A)...</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="302"/>
        <source>&amp;Second Track</source>
        <translation>두 번째 트랙(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="405"/>
        <source>&amp;Tools</source>
        <translation>도구(&amp;T)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="409"/>
        <source>&amp;Show Search</source>
        <translation>검색 표시(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="425"/>
        <source>&amp;Show Subtitle List</source>
        <translation>자막 목록 표시(&amp;L)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="445"/>
        <source>&amp;Start OCR</source>
        <translation>OCR 시작(&amp;O)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="476"/>
        <source>&amp;Settings</source>
        <translation>설정(&amp;S)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="481"/>
        <source>&amp;Anki Profile</source>
        <translation>Anki 프로필(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="518"/>
        <source>&amp;Options</source>
        <translation>옵션(&amp;O)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="524"/>
        <source>&amp;Open Config</source>
        <translation>설정 폴더 열기(&amp;C)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="531"/>
        <source>&amp;Check for Updates</source>
        <translation>업데이트 확인(&amp;U)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="536"/>
        <source>&amp;About Memento</source>
        <translation>Memento 정보(&amp;A)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="546"/>
        <source>Video Files (%1)</source>
        <translation>동영상 파일 (%1)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="547"/>
        <location filename="../qml/controls/PlayerMenu.qml" line="581"/>
        <source>All Files (*.*)</source>
        <translation>모든 파일 (*.*)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="558"/>
        <source>Open Stream</source>
        <translation>스트림 열기</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="570"/>
        <source>Enter URL</source>
        <translation>URL 입력</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="580"/>
        <source>Subtitle Files (%1)</source>
        <translation>자막 파일 (%1)</translation>
    </message>
    <message>
        <location filename="../qml/controls/PlayerMenu.qml" line="583"/>
        <source>Select Subtitle File</source>
        <translation>자막 파일 선택</translation>
    </message>
</context>
<context>
    <name>SearchPage</name>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="64"/>
        <source>Matcher</source>
        <translation>일치 방식</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="74"/>
        <source>Exact</source>
        <translation>정확 일치</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="91"/>
        <source>Deconjugation</source>
        <translation>활용 원형 복원</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="110"/>
        <source>MeCab IPAdic</source>
        <translation>MeCab IPAdic</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="125"/>
        <source>Popup</source>
        <translation>팝업</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="135"/>
        <source>Search method</source>
        <translation>검색 방식</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="142"/>
        <source>Hover</source>
        <translation>호버</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="146"/>
        <source>Modifier</source>
        <translation>보조 키</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="165"/>
        <source>Search hover millisecond delay</source>
        <translation>호버 검색 지연 시간(밀리초)</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="185"/>
        <source>Search modifier key</source>
        <translation>검색 보조 키</translation>
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
        <translation>가운데 마우스 버튼으로 검색</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="240"/>
        <source>Glossary style</source>
        <translation>뜻 표시 형식</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="247"/>
        <source>Bullet Points</source>
        <translation>글머리 기호</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="251"/>
        <source>Line Breaks</source>
        <translation>줄바꿈</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="255"/>
        <source>Pipes</source>
        <translation>파이프</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="274"/>
        <source>Auto play first result audio</source>
        <translation>첫 번째 결과 오디오 자동 재생</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="291"/>
        <source>Show add to Anki glossary checkbox</source>
        <translation>Anki에 추가할 뜻 체크박스 표시</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="307"/>
        <source>Subtitles</source>
        <translation>자막</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="317"/>
        <source>Hide mpv subtitles while Memento subtitles are visible</source>
        <translation>Memento 자막이 보이는 동안 mpv 자막 숨기기</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="334"/>
        <source>Hide Memento subtitles while playing media</source>
        <translation>미디어 재생 중 Memento 자막 숨기기</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="351"/>
        <source>Pause when hovering on subtitles</source>
        <translation>자막 위에 마우스를 올리면 일시정지</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="368"/>
        <source>Replace new lines in subtitles</source>
        <translation>자막의 줄바꿈 바꾸기</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="385"/>
        <source>New line replacement text</source>
        <translation>줄바꿈 대체 텍스트</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="402"/>
        <source>Remove subtitle text matching regex</source>
        <translation>정규식과 일치하는 자막 텍스트 제거</translation>
    </message>
    <message>
        <location filename="../qml/options/SearchPage.qml" line="406"/>
        <source>Regex</source>
        <translation>정규식</translation>
    </message>
</context>
<context>
    <name>SubtitleListPage</name>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="65"/>
        <source>Primary</source>
        <translation>기본</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="72"/>
        <source>Secondary</source>
        <translation>보조</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="233"/>
        <source>Ignore Whitespace</source>
        <translation>공백 무시</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="243"/>
        <source>Auto Seek</source>
        <translation>자동 이동</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="256"/>
        <source>Search</source>
        <translation>검색</translation>
    </message>
    <message>
        <location filename="../qml/controls/SubtitleListPage.qml" line="285"/>
        <source>No Matches</source>
        <translation>일치 항목 없음</translation>
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
        <translation>Anki 노트 추가 오류</translation>
    </message>
    <message>
        <location filename="../qml/definition/TermEntry.qml" line="251"/>
        <source>Could Not Open Anki</source>
        <translation>Anki를 열 수 없습니다</translation>
    </message>
</context>
<context>
    <name>UpdateDialog</name>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="27"/>
        <source>Update Check Failed</source>
        <translation>업데이트 확인 실패</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="28"/>
        <source>&lt;p&gt;The GitHub API did not return a valid reply.&lt;/p&gt;&lt;p&gt;Check for updates manually &lt;a href=&quot;%1&quot;&gt;here&lt;/a&gt;.</source>
        <translation>&lt;p&gt;GitHub API가 유효한 응답을 반환하지 않았습니다.&lt;/p&gt;&lt;p&gt;&lt;a href=&quot;%1&quot;&gt;여기&lt;/a&gt;에서 수동으로 업데이트를 확인하세요.</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="65"/>
        <source>Update to Date</source>
        <translation>최신 상태</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="66"/>
        <source>&lt;p&gt;You&apos;re on the latest version.&lt;/p&gt;</source>
        <translation>&lt;p&gt;최신 버전을 사용 중입니다.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="74"/>
        <source>New Version Available</source>
        <translation>새 버전 사용 가능</translation>
    </message>
    <message>
        <location filename="../qml/dialogs/UpdateDialog.qml" line="75"/>
        <source>&lt;p&gt;Version &lt;a href=&quot;%1&quot;&gt;%2&lt;/a&gt; is available.&lt;/p&gt;</source>
        <translation>&lt;p&gt;버전 &lt;a href=&quot;%1&quot;&gt;%2&lt;/a&gt;을(를) 사용할 수 있습니다.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
