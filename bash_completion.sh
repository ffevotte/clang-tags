complete -d clang-tags
function _find_completions () {
    local input="$1"
    COMPREPLY=()
    while shift; do
        local comp="$1"
        case "$comp" in
            "${input}"*)
                COMPREPLY=(${COMPREPLY[*]} "$comp")
                ;;
        esac
    done
}
function _clang_tags_complete () {
    case "$3" in
        "clang-tags")
            _find_completions "$2" "trace" "index" "update" "find-def" "grep"
            ;;
        "index")
            _find_completions "$2" "json" "scan"
            ;;
    esac
}
complete -F _clang_tags_complete clang-tags
