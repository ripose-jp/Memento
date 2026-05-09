import QtQuick
import Ripose.Memento

MetaLabel {
    id: root

    required property Tag tag

    text: root.tag?.name ?? ""
    tip: root.tag?.notes ?? ""
    color: {
        if (root.tag.category === "name")
        {
            return "#b6327a";
        }
        else if (root.tag.category === "expression")
        {
            return "#f0ad4e";
        }
        else if (root.tag.category === "popular")
        {
            return "#0275d8";
        }
        else if (root.tag.category === "frequent")
        {
            return "#5bc0de";
        }
        else if (root.tag.category === "archaism")
        {
            return "#d9534f";
        }
        else if (root.tag.category === "dictionary")
        {
            return "#aa66cc";
        }
        else if (root.tag.category === "frequency")
        {
            return "#5cb85c";
        }
        else if (root.tag.category === "partOfSpeech")
        {
            return "#565656";
        }
        else if (root.tag.category === "search")
        {
            return "#8a8a91";
        }
        else if (root.tag.category === "pitch-accent-dictionary")
        {
            return "#6640be";
        }
        else
        {
            return "#8a8a91";
        }
    }
}
