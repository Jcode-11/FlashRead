/* FlashRead Webview Client Application */

(function() {
    'use strict';

    const ALERT_ICONS = {
        note: `<svg viewBox="0 0 16 16" version="1.1"><path d="M0 8a8 8 0 1 1 16 0A8 8 0 0 1 0 8Zm8-6.5a6.5 6.5 0 1 0 0 13 6.5 6.5 0 0 0 0-13ZM6.5 7.75A.75.75 0 0 1 7.25 7h1a.75.75 0 0 1 .75.75v2.75h.25a.75.75 0 0 1 0 1.5h-2a.75.75 0 0 1 0-1.5h.25v-2h-.25a.75.75 0 0 1-.75-.75ZM8 6a1 1 0 1 1 0-2 1 1 0 0 1 0 2Z"></path></svg>`,
        tip: `<svg viewBox="0 0 16 16" version="1.1"><path d="M8 1.5c-2.363 0-4 1.69-4 3.75 0 .984.424 1.625.984 2.304l.214.253c.223.264.47.556.673.848.284.411.537.896.621 1.49a.75.75 0 0 1-1.484.211c-.04-.282-.163-.547-.37-.847a8.456 8.456 0 0 0-.542-.68c-.09-.107-.18-.214-.27-.32C3.2 7.784 2.5 6.74 2.5 5.25 2.5 2.31 4.863 0 8 0s5.5 2.31 5.5 5.25c0 1.49-.7 2.534-1.326 3.272a8.456 8.456 0 0 0-.812 1c-.207.3-.33.565-.37.847a.75.75 0 0 1-1.484-.212c.084-.593.337-1.078.621-1.489.203-.292.45-.584.673-.848.075-.088.147-.173.213-.253.561-.679.985-1.32.985-2.304 0-2.06-1.637-3.75-4-3.75ZM5.75 12h4.5a.75.75 0 0 1 0 1.5h-4.5a.75.75 0 0 1 0-1.5Zm1 3h2.5a.75.75 0 0 1 0 1.5h-2.5a.75.75 0 0 1 0-1.5Z"></path></svg>`,
        important: `<svg viewBox="0 0 16 16" version="1.1"><path d="M0 1.75C0 .784.784 0 1.75 0h12.5C15.216 0 16 .784 16 1.75v9.5A1.75 1.75 0 0 1 14.25 13H9.06l-2.573 2.573A1.458 1.458 0 0 1 4 14.543V13H1.75A1.75 1.75 0 0 1 0 11.25Zm1.75-.25a.25.25 0 0 0-.25.25v9.5c0 .138.112.25.25.25h3a.75.75 0 0 1 .75.75v2.19l2.72-2.72a.749.749 0 0 1 .53-.22h5.5a.25.25 0 0 0 .25-.25v-9.5a.25.25 0 0 0-.25-.25Zm6.47 2.22a.75.75 0 0 1 .78 0 .75.75 0 0 1 .38.65v3a.75.75 0 0 1-1.5 0v-3a.75.75 0 0 1 .34-.65Zm0 6.03a1 1 0 1 1 0-2 1 1 0 0 1 0 2Z"></path></svg>`,
        warning: `<svg viewBox="0 0 16 16" version="1.1"><path d="M6.457 1.047c.659-1.234 2.427-1.234 3.086 0l6.082 11.378A1.75 1.75 0 0 1 14.082 15H1.918a1.75 1.75 0 0 1-1.543-2.575Zm1.763.707a.25.25 0 0 0-.44 0L1.698 13.132a.25.25 0 0 0 .22.368h12.164a.25.25 0 0 0 .22-.368Zm.53 3.996v2.5a.75.75 0 0 1-1.5 0v-2.5a.75.75 0 0 1 1.5 0ZM9 11a1 1 0 1 1-2 0 1 1 0 0 1 2 0Z"></path></svg>`,
        caution: `<svg viewBox="0 0 16 16" version="1.1"><path d="M4.47.22A.749.749 0 0 1 5 0h6c.199 0 .389.079.53.22l4.25 4.25c.141.14.22.331.22.53v6a.749.749 0 0 1-.22.53l-4.25 4.25A.749.749 0 0 1 11 16H5a.749.749 0 0 1-.53-.22L.22 11.53A.749.749 0 0 1 0 11V5c0-.199.079-.389.22-.53Zm.84 1.28L1.5 5.31v5.38l3.81 3.81h5.38l3.81-3.81V5.31L10.69 1.5ZM8 4a.75.75 0 0 1 .75.75v3.5a.75.75 0 0 1-1.5 0v-3.5A.75.75 0 0 1 8 4Zm0 8a1 1 0 1 1 0-2 1 1 0 0 1 0 2Z"></path></svg>`
    };

    function postToHost(msg) {
        if (window.chrome && window.chrome.webview) {
            window.chrome.webview.postMessage(msg);
        }
    }

    function enhanceCodeBlocks(container) {
        const preElements = container.querySelectorAll('pre');
        preElements.forEach((pre) => {
            if (pre.closest('.code-card')) return;

            const code = pre.querySelector('code');
            const rawText = code ? code.textContent : pre.textContent;
            
            let lang = 'text';
            if (code && code.className) {
                const match = code.className.match(/(?:language|lang)-([\w+-]+)/i);
                if (match) lang = match[1].toLowerCase();
            }

            let prismLang = lang;
            if (lang === 'c++' || lang === 'cpp' || lang === 'cc' || lang === 'cxx' || lang === 'h' || lang === 'hpp') prismLang = 'cpp';
            else if (lang === 'py' || lang === 'python3') prismLang = 'python';
            else if (lang === 'js' || lang === 'mjs' || lang === 'cjs') prismLang = 'javascript';
            else if (lang === 'ts') prismLang = 'typescript';
            else if (lang === 'rs') prismLang = 'rust';
            else if (lang === 'sh' || lang === 'shell' || lang === 'zsh') prismLang = 'bash';
            else if (lang === 'yml') prismLang = 'yaml';
            else if (lang === 'golang') prismLang = 'go';

            // Generate line numbers
            const lines = rawText.replace(/\r\n/g, '\n').replace(/\r/g, '\n').split('\n');
            if (lines.length > 0 && lines[lines.length - 1] === '') {
                lines.pop();
            }
            const lineCount = Math.max(1, lines.length);
            let lineNumsHtml = '';
            for (let i = 1; i <= lineCount; ++i) {
                lineNumsHtml += i + '\n';
            }

            // Create card wrapper
            const card = document.createElement('div');
            card.className = 'code-card';

            const header = document.createElement('div');
            header.className = 'code-card-header';

            const headerLeft = document.createElement('div');
            headerLeft.className = 'code-card-header-left';

            const dots = document.createElement('div');
            dots.className = 'mac-dots';
            dots.innerHTML = '<span class="mac-dot red"></span><span class="mac-dot yellow"></span><span class="mac-dot green"></span>';

            const tag = document.createElement('div');
            tag.className = 'code-lang-tag';
            tag.innerHTML = `<span>${lang}</span>`;

            headerLeft.appendChild(dots);
            headerLeft.appendChild(tag);

            const copySvg = `<svg viewBox="0 0 16 16" version="1.1"><path d="M0 6.75C0 5.784.784 5 1.75 5h1.5a.75.75 0 0 1 0 1.5h-1.5a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 0 0 .25-.25v-1.5a.75.75 0 0 1 1.5 0v1.5A1.75 1.75 0 0 1 9.25 16h-7.5A1.75 1.75 0 0 1 0 14.25Z"></path><path d="M5 1.75C5 .784 5.784 0 6.75 0h7.5C15.216 0 16 .784 16 1.75v7.5A1.75 1.75 0 0 1 14.25 11h-7.5A1.75 1.75 0 0 1 5 9.25Zm1.75-.25a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 0 0 .25-.25v-7.5a.25.25 0 0 0-.25-.25Z"></path></svg>`;

            const copyBtn = document.createElement('button');
            copyBtn.className = 'code-copy-btn';
            copyBtn.innerHTML = `${copySvg}<span>Copy</span>`;
            copyBtn.setAttribute('title', '复制代码');

            copyBtn.addEventListener('click', () => {
                navigator.clipboard.writeText(rawText).then(() => {
                    copyBtn.classList.add('copied');
                    copyBtn.innerHTML = `<span>✓</span><span>Copied!</span>`;
                    setTimeout(() => {
                        copyBtn.classList.remove('copied');
                        copyBtn.innerHTML = `${copySvg}<span>Copy</span>`;
                    }, 2000);
                }).catch(err => {
                    console.error('Clipboard copy failed:', err);
                });
            });

            header.appendChild(headerLeft);
            header.appendChild(copyBtn);

            const body = document.createElement('div');
            body.className = 'code-card-body';

            const lineNumsCol = document.createElement('div');
            lineNumsCol.className = 'line-numbers-col';
            lineNumsCol.textContent = lineNumsHtml;

            const contentCol = document.createElement('div');
            contentCol.className = 'code-content-col';

            const newPre = document.createElement('pre');
            const newCode = document.createElement('code');
            newCode.className = `language-${prismLang}`;
            newCode.textContent = rawText;
            newPre.appendChild(newCode);
            contentCol.appendChild(newPre);

            body.appendChild(lineNumsCol);
            body.appendChild(contentCol);

            card.appendChild(header);
            card.appendChild(body);

            pre.parentNode.replaceChild(card, pre);

            // Highlight with Prism
            if (window.Prism && Prism.languages[prismLang]) {
                Prism.highlightElement(newCode);
            } else if (window.Prism && Prism.languages.clike) {
                Prism.highlightElement(newCode);
            }
        });
    }

    function enhanceCallouts(container) {
        const quotes = container.querySelectorAll('blockquote');
        quotes.forEach(quote => {
            const firstP = quote.querySelector('p');
            if (!firstP) return;

            const text = firstP.textContent.trim();
            const match = text.match(/^\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\]/i);
            if (match) {
                const type = match[1].toLowerCase();
                quote.className = `markdown-alert markdown-alert-${type}`;
                
                // Remove [!TYPE] from text
                firstP.innerHTML = firstP.innerHTML.replace(/^\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\]\s*<br\s*\/?>?/i, '')
                                                 .replace(/^\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\]\s*/i, '');
                
                // Add header with icon
                const header = document.createElement('div');
                header.className = 'markdown-alert-title';
                header.innerHTML = `${ALERT_ICONS[type] || ''} <span>${type.toUpperCase()}</span>`;
                quote.insertBefore(header, firstP);
            }
        });
    }

    let headingObserver = null;
    function setupHeadingObserver(container) {
        if (headingObserver) {
            headingObserver.disconnect();
        }

        const headings = container.querySelectorAll('h1, h2, h3');
        if (headings.length === 0) return;

        headingObserver = new IntersectionObserver((entries) => {
            for (const entry of entries) {
                if (entry.isIntersecting) {
                    const id = entry.target.id || entry.target.textContent.trim();
                    postToHost({ type: 'headingVisible', id: id });
                    break;
                }
            }
        }, {
            root: null,
            rootMargin: '0px 0px -70% 0px',
            threshold: 0.1
        });

        headings.forEach(h => {
            if (!h.id) {
                h.id = 'heading-' + encodeURIComponent(h.textContent.trim().toLowerCase().replace(/\s+/g, '-'));
            }
            headingObserver.observe(h);
        });
    }

    // Intercept clicks on links
    document.addEventListener('click', (e) => {
        const a = e.target.closest('a');
        if (!a) return;

        const href = a.getAttribute('href');
        if (!href) return;

        if (href.startsWith('#')) {
            e.preventDefault();
            const targetId = href.substring(1);
            const targetEl = document.getElementById(targetId) || document.querySelector(`[name="${targetId}"]`);
            if (targetEl) {
                targetEl.scrollIntoView({ behavior: 'smooth' });
            }
        } else if (href.startsWith('http://') || href.startsWith('https://') || href.startsWith('mailto:')) {
            e.preventDefault();
            postToHost({ type: 'openExternal', url: href });
        }
    });

    // Scroll progress reporter
    let scrollTimeout = null;
    window.addEventListener('scroll', () => {
        if (scrollTimeout) return;
        scrollTimeout = setTimeout(() => {
            scrollTimeout = null;
            const scrollMax = document.documentElement.scrollHeight - window.innerHeight;
            if (scrollMax > 0) {
                const progress = Math.max(0, Math.min(1.0, window.scrollY / scrollMax));
                postToHost({ type: 'scrollProgress', progress: progress });
            }
        }, 60);
    });

    // Public API for C++ host
    window.updateDocument = function(htmlContent, themeId, scrollPos) {
        const container = document.getElementById('markdown-container');
        if (!container) return;

        if (themeId) {
            document.documentElement.setAttribute('data-theme', themeId);
        }

        container.innerHTML = htmlContent;
        enhanceCodeBlocks(container);
        enhanceCallouts(container);
        setupHeadingObserver(container);

        if (typeof scrollPos === 'number' && scrollPos >= 0) {
            window.scrollTo({ top: scrollPos, behavior: 'instant' });
        }
    };

    window.setTheme = function(themeId) {
        document.documentElement.setAttribute('data-theme', themeId);
    };

    window.scrollToHeading = function(titleOrId) {
        const headings = document.querySelectorAll('h1, h2, h3, h4, h5, h6');
        for (const h of headings) {
            if (h.id === titleOrId || h.textContent.trim() === titleOrId.trim()) {
                h.scrollIntoView({ behavior: 'smooth' });
                return true;
            }
        }
        return false;
    };

    window.scrollToProgress = function(progress) {
        const scrollMax = document.documentElement.scrollHeight - window.innerHeight;
        if (scrollMax > 0) {
            window.scrollTo({ top: scrollMax * progress, behavior: 'smooth' });
        }
    };

    if (window.__latestDoc && window.__latestDoc.html) {
        window.updateDocument(window.__latestDoc.html, window.__latestDoc.theme);
    }

    // Ready signal
    postToHost({ type: 'webviewReady' });

})();
