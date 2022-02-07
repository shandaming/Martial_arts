" An example for a vimrc file.
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2016 Mar 25
"
" To use it, copy it to
"     for Unix and OS/2:  ~/.vimrc
"	      for Amiga:  s:.vimrc
"  for MS-DOS and Win32:  $VIM\_vimrc
"	    for OpenVMS:  sys$login:.vimrc

" When started as "evim", evim.vim will already have done these settings.
if v:progname =~? "evim"
  finish
endif

" Use Vim settings, rather than Vi settings (much better!).
" This must be first, because it changes other options as a side effect.
set nocompatible

" allow backspacing over everything in insert mode
set backspace=indent,eol,start

" if has("vms")
"   set nobackup		" do not keep a backup file, use versions instead
" else
"   set backup		" keep a backup file (restore to previous version)
"   set undofile		" keep an undo file (undo changes after closing)
" endif

set history=50		" keep 50 lines of command line history
set ruler		" show the cursor position all the time
set showcmd		" display incomplete commands


" For Win32 GUI: remove 't' flag from 'guioptions': no tearoff menu entries
" let &guioptions = substitute(&guioptions, "t", "", "g")

" Don't use Ex mode, use Q for formatting
map Q gq

" CTRL-U in insert mode deletes a lot.  Use CTRL-G u to first break undo,
" so that you can undo CTRL-U after inserting a line break.
inoremap <C-U> <C-G>u<C-U>

" In many terminal emulators the mouse works just fine, thus enable it.
if has('mouse')
  set mouse=a
endif

" Switch syntax highlighting on, when the terminal has colors
" Also switch on highlighting the last used search pattern.
if &t_Co > 2 || has("gui_running")
  syntax on
  set hlsearch
endif

" Only do this part when compiled with support for autocommands.
if has("autocmd")

  " Enable file type detection.
  " Use the default filetype settings, so that mail gets 'tw' set to 72,
  " 'cindent' is on in C files, etc.
  " Also load indent files, to automatically do language-dependent indenting.
  filetype plugin indent on

  " Put these in an autocmd group, so that we can delete them easily.
  augroup vimrcEx
  au!

  " For all text files set 'textwidth' to 78 characters.
  autocmd FileType text setlocal textwidth=78

  " When editing a file, always jump to the last known cursor position.
  " Don't do it when the position is invalid or when inside an event handler
  " (happens when dropping a file on gvim).
  autocmd BufReadPost *
    \ if line("'\"") >= 1 && line("'\"") <= line("$") |
    \   exe "normal! g`\"" |
    \ endif

  augroup END

else

  set autoindent		" always set autoindenting on

endif " has("autocmd")

" Convenient command to see the difference between the current buffer and the
" file it was loaded from, thus the changes you made.
" Only define it when not defined already.
if !exists(":DiffOrig")
  command DiffOrig vert new | set bt=nofile | r ++edit # | 0d_ | diffthis
		  \ | wincmd p | diffthis
endif

if has('langmap') && exists('+langnoremap')
  " Prevent that the langmap option applies to characters that result from a
  " mapping.  If unset (default), this may break plugins (but it's backward
  " compatible).
  set langnoremap
endif


" Add optional packages.
"
" The matchit plugin makes the % command work better, but it is not backwards
" compatible.
packadd matchit


" @refer http://vimcdoc.sourceforge.net/doc/help.html


" ********************** 中文参考 http://vimcdoc.sourceforge.net/doc/help.html ***********************************
set nocompatible						" 不兼容VI
set noundofile							" 不保存撤销历史文件记录
set nobackup							" 不创建备份文件
set nowritebackup						" 不创建备份文件缺省行为
set noswapfile							" 不创建交换文件
set autochdir							" Vim 会在你打开文件、切换缓冲区、删除缓冲区或者打开/关闭窗口时改变当前工作目录的值
set autoindent							" 根据上一行决定新行的缩进
set browsedir=current					" 开始浏览文件的目录{仅适用于 Motif、Athena、GTK、Mac 和 Win32 GUI}
set cursorline							" 高亮当前行
set encoding=utf-8						" 内部使用的编码方式
set hlsearch							" 高亮最近的匹配搜索模式
set incsearch							" 输入搜索模式时同时高亮部分的匹配
set laststatus=2						" 当最近的窗口有状态行时提示
set ruler								" 打开状态栏标尺
set showcmd								" 显示历史命令
set history=45							" 最多保存50行命令记录
set number								" 行前显示行号
set t_Co=256							" 使用256颜色
color custom_color						" 设置主题
set wildmenu							" 启用增强的Tab自动补全
set wildmode=list:longest,full			" 补全为允许的最长字符串,然后打开 wildmenu
set clipboard=unamed,unnamedplus		" 复制到系统寄存器(*, +)

set cindent								" 实现 C 程序的缩进
set cinoptions=N-s,g0					" c++ namespace无缩进
set smartindent							" C 程序智能自动缩进
syntax on								" 打开语法高亮
set nofoldenable						" 打开所有折叠
set foldmethod=indent					" 折叠的类型


" set expandtab							" 键入 <Tab> 时使用空格
set tabstop=4							" <Tab> 在文件里使用的空格数
set shiftwidth=4						" (自动) 缩进使用的步进单位，以空白数目计
" set softtabstop=4						" 编辑时 <Tab> 使用的空格数

filetype plugin indent on				"启用根据文件类型自动缩进

" 使用<Ctrl> + hjkl 快捷键在窗口间跳转
noremap <c-h> <c-w><c-h>
noremap <c-j> <c-w><c-j>
noremap <c-k> <c-w><c-k>
noremap <c-l> <c-w><c-l>


" ----------------------------------- vim-plug START -----------------------------------------

call plug#begin('~/.vim/plugged')

Plug 'preservim/nerdtree', {'tag': '6.10.16'}
Plug 'neoclide/coc.nvim', {'branch': 'release'}

" Initialize plugin system
call plug#end()

" ----------------------------------- vim-plug END -----------------------------------------

" ----------------------------------- nerdtree STAR ----------------------------------------

" 启动 NERDTree 并将光标放回另一个窗口。
autocmd VimEnter * NERDTree | wincmd p 

" 在没有文件参数的情况下启动 Vim 时启动 NERDTree。 
autocmd StdinReadPre * let s:std_in=1
autocmd VimEnter * if argc() == 0 && !exists('s:std_in') | NERDTree | endif

" 启动 NERDTree。如果指定了文件，则将光标移动到其窗口。 
autocmd StdinReadPre * let s:std_in=1
autocmd VimEnter * NERDTree | if argc() > 0 || exists("s:std_in") | wincmd p | endif

" 如果 NERDTree 是唯一选项卡中剩余的唯一窗口，则退出 Vim。 
autocmd BufEnter * if tabpagenr('$') == 1 && winnr('$') == 1 && exists('b:NERDTree') && b:NERDTree.isTabTree() | quit | endif

" 如果 NERDTree 是其中唯一剩余的窗口，则关闭选项卡。 
autocmd BufEnter * if winnr('$') == 1 && exists('b:NERDTree') && b:NERDTree.isTabTree() | quit | endif

" 如果另一个缓冲区试图替换 NERDTree，请将其放在另一个窗口中，然后带回 NERDTree。autocmd 
autocmd BufEnter * if bufname('#') =~ 'NERD_tree_\d\+' && bufname('%') !~ 'NERD_tree_\d\+' && winnr('$') > 1 |
    \ let buf=bufnr() | buffer# | execute "normal! \<C-W>w" | execute 'buffer'.buf | endif

" ----------------------------------- nerdtree END -----------------------------------------

set completeopt=menu					"关闭preview window



" ----------------------------------- c0c.nvim ----------------------------------------------------



set updatetime=300		" 更长的更新时间（默认为 4000 ms = 4 s）会导致明显的延迟和糟糕的用户体验。 
set shortmess+=c		" 不要将消息传递给 |ins-completion-menu| 

if has("nvim-0.5.0") || has("patch-8.1.1564")		" 始终显示符号列，否则每次诊断出现/解决时它都会改变文本。
  " 最近vim可以把signcolumn和number column合二为一 
  set signcolumn=number
else
  set signcolumn=yes
endif


" 使 <CR> 自动选择第一个补全项并在进入时通知 coc.nvim 格式化，<cr> 可以被其他 vim 插件重新映射 
inoremap <silent><expr> <cr> pumvisible() ? coc#_select_confirm()
                              \: "\<C-g>u\<CR>\<c-r>=coc#on_enter()\<CR>"

" 使用 `[g` 和 `]g` 导航诊断
" 使用 `:Coco Diagnostics` 获取位置列表中当前缓冲区的所有诊断信息。 
nmap <silent> [g <Plug>(coc-diagnostic-prev)
nmap <silent> ]g <Plug>(coc-diagnostic-next)

" GoTo 代码导航。 
nmap <silent> gd <Plug>(coc-definition)
nmap <silent> gy <Plug>(coc-type-definition)
nmap <silent> gi <Plug>(coc-implementation)
nmap <silent> gr <Plug>(coc-references)

" 按住光标时突出显示符号及其引用。
autocmd CursorHold * silent call CocActionAsync('highlight')

" 符号重命名。
nmap <leader>rn <Plug>(coc-rename)

" 将代码操作应用于所选区域。
" 示例：当前段落的 `<leader>app` 
xmap <leader>a  <Plug>(coc-codeaction-selected)
nmap <leader>a  <Plug>(coc-codeaction-selected)

" 重新映射键以将 codeAction 应用于当前缓冲区。 
nmap <leader>ac  <Plug>(coc-codeaction)
" 将 AutoFix 应用于当前行上的问题。 
nmap <leader>qf  <Plug>(coc-fix-current)

" 在当前行上运行 Code Lens 操作。 
nmap <leader>cl  <Plug>(coc-codelens-action)


" 为滚动浮动窗口/弹出窗口重新映射 <C-f> 和 <C-b>。 
if has('nvim-0.4.0') || has('patch-8.2.0750')
  nnoremap <silent><nowait><expr> <C-f> coc#float#has_scroll() ? coc#float#scroll(1) : "\<C-f>"
  nnoremap <silent><nowait><expr> <C-b> coc#float#has_scroll() ? coc#float#scroll(0) : "\<C-b>"
  inoremap <silent><nowait><expr> <C-f> coc#float#has_scroll() ? "\<c-r>=coc#float#scroll(1)\<cr>" : "\<Right>"
  inoremap <silent><nowait><expr> <C-b> coc#float#has_scroll() ? "\<c-r>=coc#float#scroll(0)\<cr>" : "\<Left>"
  vnoremap <silent><nowait><expr> <C-f> coc#float#has_scroll() ? coc#float#scroll(1) : "\<C-f>"
  vnoremap <silent><nowait><expr> <C-b> coc#float#has_scroll() ? coc#float#scroll(0) : "\<C-b>"
endif

" 使用 CTRL-S 选择范围。
" 需要语言服务器的 'textDocument/selectionRange' 支持。 
nmap <silent> <C-s> <Plug>(coc-range-select)
xmap <silent> <C-s> <Plug>(coc-range-select)

" CoCList 的映射
" 显示所有诊断。 
nnoremap <silent><nowait> <space>a  :<C-u>CocList diagnostics<cr>
" 管理扩展。 
nnoremap <silent><nowait> <space>e  :<C-u>CocList extensions<cr>
" 显示命令。
nnoremap <silent><nowait> <space>c  :<C-u>CocList commands<cr>
" 查找当前文档的符号。
nnoremap <silent><nowait> <space>o  :<C-u>CocList outline<cr>
" 搜索工作区符号。 
nnoremap <silent><nowait> <space>s  :<C-u>CocList -I symbols<cr>
" 为下一项执行默认操作。
nnoremap <silent><nowait> <space>j  :<C-u>CocNext<CR>
" 对上一个项目执行默认操作。 
nnoremap <silent><nowait> <space>k  :<C-u>CocPrev<CR>
" 恢复最新的 coc 列表。 
nnoremap <silent><nowait> <space>p  :<C-u>CocListResume<CR>

" ----------------------------------- c0c.nvim ----------------------------------------------------

