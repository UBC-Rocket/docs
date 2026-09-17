window.MathJax = {
    loader: {
        load: ["[tex]/cancel", "[tex]/mathtools", "[tex]/physics"],
    },
    tex: {
        packages: { "[+]": ["cancel", "mathtools", "physics"] },
        inlineMath: [["\\(", "\\)"]],
        displayMath: [["\\[", "\\]"]],
        processEscapes: true,
        processEnvironments: true,
    },
    options: {
        ignoreHtmlClass: ".*|",
        processHtmlClass: "arithmatex",
    },
};

document$.subscribe(() => {
    MathJax.startup.output.clearCache();
    MathJax.typesetClear();
    MathJax.texReset();
    MathJax.typesetPromise();
});
