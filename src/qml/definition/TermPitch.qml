import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Ripose.Memento

ColumnLayout {
    id: root

    required property Pitch pitch

    Repeater {
        model: root.pitch?.positions ?? []
        delegate: RowLayout {
            id: pitchRow

            readonly property int position: modelData

            MetaLabel {
                text: `${root.pitch?.dictionaryInfo?.name ?? ""}`
                color: "#6640be"
            }

            SelectableLabel {
                id: pitchLabel

                font.family: MementoSettings.interfaceSearchGlossaryFont.family
                font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                font.letterSpacing: 2
                text: root.pitch?.mora.join("") ?? ""
                onTextChanged: canvas.requestPaint()

                Canvas {
                    id: canvas
                    anchors.fill: parent
                    onPaint: {
                        const ctx = getContext("2d");
                        ctx.strokeStyle = pitchLabel.color;
                        ctx.lineWidth = 2;

                        const correctionFactor = pitchLabel.font.letterSpacing / 2;

                        if (!root.pitch)
                        {
                            return;
                        }

                        ctx.beginPath();
                        switch (pitchRow.position)
                        {
                        case 0:
                        {
                            const firstMoraLength = root.pitch.mora[0].length;
                            const firstMoraRect = pitchLabel.positionToRectangle(firstMoraLength);
                            ctx.moveTo(0, pitchLabel.height);
                            ctx.lineTo(firstMoraRect.x - correctionFactor, pitchLabel.height);
                            ctx.lineTo(firstMoraRect.x - correctionFactor, 0);
                            if (root.pitch.mora.length > 1)
                            {
                                const endMoraRect = pitchLabel.positionToRectangle(pitchLabel.text.length);
                                ctx.lineTo(endMoraRect.x - correctionFactor, 0);
                            }
                            break;
                        }

                        case 1:
                        {
                            const firstMoraLength = root.pitch.mora[0].length;
                            const firstMoraRect = pitchLabel.positionToRectangle(firstMoraLength);
                            ctx.moveTo(0, 0);
                            ctx.lineTo(firstMoraRect.x - correctionFactor, 0);
                            ctx.lineTo(firstMoraRect.x - correctionFactor, pitchLabel.height);
                            if (root.pitch.mora.length > 1)
                            {
                                const endMoraRect = pitchLabel.positionToRectangle(pitchLabel.text.length);
                                ctx.lineTo(endMoraRect.x - correctionFactor, pitchLabel.height);
                            }
                            break;
                        }

                        default:
                        {
                            if (root.pitch.mora.length < pitchRow.position)
                            {
                                break;
                            }

                            const firstMoraLength = root.pitch.mora[0].length;
                            const firstMoraRect = pitchLabel.positionToRectangle(firstMoraLength);
                            ctx.moveTo(0, pitchLabel.height);
                            ctx.lineTo(firstMoraRect.x - correctionFactor, pitchLabel.height);
                            ctx.lineTo(firstMoraRect.x - correctionFactor, 0);

                            let fallIndex = 0;
                            for (let i = 0; i < pitchRow.position; ++i)
                            {
                                fallIndex += root.pitch.mora[i].length;
                            }
                            const fallMoraRect = pitchLabel.positionToRectangle(fallIndex);
                            ctx.lineTo(fallMoraRect.x - correctionFactor, 0);
                            ctx.lineTo(fallMoraRect.x - correctionFactor, pitchLabel.height);

                            if (root.pitch.mora.length > pitchRow.position)
                            {
                                const endMoraRect = pitchLabel.positionToRectangle(pitchLabel.text.length);
                                ctx.lineTo(endMoraRect.x - correctionFactor, pitchLabel.height);
                            }
                            break;
                        }
                        }
                        ctx.stroke();
                    }
                }
            }

            Label {
                font.family: MementoSettings.interfaceSearchGlossaryFont.family
                font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                text: `[${pitchRow.position}]`
            }
        }
    }
}
