let qubits
let circuit_gates

$(function(){

  /* DISPLAY NICETIES */

  /* Ensure right-column panels don't overlap */
  $('.panel-control .summoner').mouseover(function(){
    let anchor = $(this).children('.summon-anchor')
    let anchor_right = $(window).width() - anchor.offset().left
    let right = anchor_right - anchor.width() - 10
    $(this).children('.summoned').css("right", `${right}px`)
  })

  let percent = (x, d) => String(Math.floor(x*100 * 10**d)/10**d)+"%"
  let fadeAt = function(x, limit, min){
    /* If x is above the limit, return 1; if below, fade linearly to the minimum. */
    return x > limit ? 1 : min + (x/limit)*(1-min)
  }

  /* PANEL */
  qubits = Array()

  let conf_default = function(){return {
    noise: 0,
    shots: 1024,
    isSparse: false,
    doGroup: true,
    emitStates: true,
  }}

  let conf = conf_default();

  const NOISE_SIZE = 10000

  let refreshConf = function(){
    conf.shots = 2 ** $('#slider-shots').val()
    conf.noise = $('#slider-noise').val() / NOISE_SIZE
    conf.isSparse = $('#check-sparse').is(':checked')
    conf.doGroup = $('#check-group').is(':checked')

    $('.disp-qubits').text(qubits.length)
    $('.disp-shots').text(conf.shots)
    let fragments = [
      `${conf.shots} shots`
    ]
    $('.disp-noise-full').text(
      conf.noise ? "a " + percent(conf.noise, 2)
                 : "no") // chance for...
    conf.noise ? fragments.push(percent(conf.noise, 0) +" decay") :0;

    $('.disp-summary').text(fragments.join(", "))
    $('#btn-qubit-less').prop("disabled", qubits.length <3)
  }

  let addQubit = function(){
    let n = qubits.length
    let qubit = {
      gates_used: [],
      el: $(`
      <tr class='qubit'>
        <td class='header'>
          q[${n}]<span class='plus'>+</span>
        </td>
        <td class='initial'>
          \\(\\ket{0}\\)
        </td>
      </tr>`)
      .appendTo('#qubits')
    }
    MathJax.typeset()
    qubit.el.children('.header').click(()=>select_qubit(n))
    qubits.push(qubit)
  };
  $('#btn-qubit-more').click(function(){
    addQubit()
    refreshConf()
    refreshGates()
  });

  const MATHJAX_URL = "https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"
  $.getScript(MATHJAX_URL, function(){
    setTimeout(function(){
      $('html').addClass('mathjax-loaded')
      $('#btn-qubit-less').prop("disabled", false);
      $('#btn-qubit-more').prop("disabled", false);
      addQubit(); addQubit()
      refreshConf()
      refreshGates()
      refreshSelector();
    }, 100)
  })
  

  $('#btn-qubit-less').click(function(){
    if (qubits.length > 2) {
      let line = qubits.pop()
      let lineIsEmpty = true
      if (lineIsEmpty) {
        line.el.remove()
      } else {
        qubits.push(line)
      }
      refreshConf()
      refreshGates()
    }
  })

  $('#slider-shots').change(refreshConf)
  $('#slider-shots').mousemove(refreshConf)
  $('#slider-noise').change(refreshConf)
  $('#slider-noise').mousemove(refreshConf)

  $('#check-sparse').change(refreshConf)
  $('#check-group').change(refreshConf)

  /* GATES */
  circuit_gates = []

  let newSel = ()=>Object({
    gate: null,
    args: [],
    cargs: []
  })
  sel = newSel()

  $('#btn-reset').click(function(){
    sel = newSel()
    circuit_gates = [];
    conf = conf_default();
    refreshConf();
    refreshGates();
    refreshSelector();
  })

  const ORDINAL = [
    '1<sup>st</sup>',
    '2<sup>nd</sup>'
  ]

  let refreshSelector = function(){
    for (let [i, gate] of Object.entries(GATES)) {
      gate.el.removeClass('selected')
    }
    let msg = "No gate selected."
    let msg_qubit = "Click one to start."
    $('.builder').removeClass('selection')
    $('#circuit-add-gate').addClass('hidden')

    if( sel.gate ){
      let q = GATES[sel.gate]
      msg = `<strong>${q.el.find('h2').html()}</strong>:`
      q.el.addClass('selected');
      $('.builder').addClass('selection');

      const selecting_control_qubit = sel.args.length == q.arity
      if( selecting_control_qubit ){
        const can_add_now = q.control_arities.includes(sel.cargs.length)
        if( can_add_now ){
          $('#circuit-add-gate').removeClass('hidden')
        }
        msg_qubit = (can_add_now ? "or s" : "S") + "elect the "
        if ( String(q.control_arities) != "0,1" ){
          msg_qubit += `${ORDINAL[sel.cargs.length]} `
        }
        msg_qubit += "control qubit."
      } else {
        msg_qubit = "Select the "
        if( q.arity > 1 ){
          msg_qubit += `${ORDINAL[sel.args.length]} `
        }
        msg_qubit += "qubit to act on."
      }
    }
    $('#status').html(msg)
    $('#status-qubit').html(msg_qubit)
  }

  /* First click selects (or resets) adding a gate. */
  $.each(GATES, function(id, op){
    op.el = $(`#${id}`)
    op.el.click(function(){
      sel.gate = sel.gate == id ? null : id
      refreshSelector()
      op.el.addClass('summon-shushed')
    })
    op.el.mouseout(function(){
      op.el.removeClass('summon-shushed')
    })
  })

  /* Selecting a qubit (for now) adds the gate to the end of that qubit. */
  /* This is naturally temporary to bootstrap the rest of functionality. */
  let select_qubit = function(q, index){
    if( !sel.gate ){ return; }
    if( sel.args.includes(q) || sel.cargs.includes(q) ){
      if( !tryAddGate() ){
        console.log(`Attempted to select qubit ${q} (already added)`)
      }
      return;
    }
    let gate = GATES[sel.gate]
    let c = gate.control_arities

    if( sel.args.length == 0 && index !== undefined){
      console.log("index", index)
      sel.index = index
    }

    if( sel.args.length < gate.arity ){
      sel.args.push(q)
    } else {
      sel.cargs.push(q)
    }

    console.log("gate", gate)

    /* allow optional extra control qubits */
    if( c && sel.cargs.length == c[c.length - 1] ){
      tryAddGate()
    } else if( sel.args.length + sel.cargs.length == qubits.length ){
      tryAddGate()
    }
    refreshSelector()
  }

  let tryAddGate = function(){
    /* Try to add the working gate. Will fail gracefully if not complete. */
    if( sel.gate ){
      let q = GATES[sel.gate]
      if (sel.args.length == q.arity && sel.cargs.length in q.control_arities){
        if ( !isNaN(sel.index) ){
          circuit_gates.splice(sel.index, 0, sel)
        } else {
          circuit_gates.push(sel)
        }
        console.log(sel)
        sel.all_args = sel.cargs.concat(sel.args)
        for (let i = 0; i < sel.all_args.length; i++) {
          const qindex = sel.all_args[i];
          qubits[qindex].gates_used.push(sel)
        }
        sel = newSel()
        refreshSelector()
        refreshGates()
        return true
      }
    }
    return false;
  }
  $('#circuit-add-gate button').click(tryAddGate)

  let refreshGates = function(){    
    $('.gate').remove()
    let column = []
    let colFinished = function(){
      for (let q = 0; q < qubits.length; q++) {
        const ico = column[q] || {
          clsOuter: ["gate"], symbol: "",
          clsMiddle: ["placed-gate"],
          clsInner: ["gate-icon", "empty"]
        }
        let is = x => ico.clsInner.includes(x);
        let hasControl = !is("empty") && !is("control-dot")
        let control = hasControl ? `
            <div class='gate-control'>
              <div class='delete'>×</div>
              <div class='add'>+</div>
            </div>` : ""
        let el = $(`
          <td class="${ico.clsOuter.join(' ')}">
            <div class="${ico.clsMiddle.join(' ')}">
              <div class="${ico.clsInner.join(' ')}">
                ${ico.symbol}
              </div>
              ${control}
            </div>
          </td>`).appendTo(qubits[q].el)
        if (hasControl) {
          el.find('.delete').click(function(){
            let a = circuit_gates[ico.g]
            console.log(a)
            circuit_gates.splice(ico.g, 1)
            refreshGates()
          })
          el.find('.add').click(function(){
            select_qubit(q, ico.g)
          })
        }
      }
      column = []
    }
    for (let g = 0; g < circuit_gates.length; g++) {
      const gate = circuit_gates[g];
      let gateType = GATES[gate.gate]
      const g_min_q = Math.min.apply(0, gate.all_args)
      const g_max_q = Math.max.apply(0, gate.all_args)

      for (let q = g_min_q; q <= g_max_q; q++) {
        if( column[q] ){
          /* would overlap with previous column! */
          colFinished()
          break;
        }
      }
      
      for (let q = g_min_q; q <= g_max_q; q++) {
        let ico = {
          g: g,
          clsOuter: ["gate", gateType.kind],
          clsMiddle: ["placed-gate"],
          clsInner: ["gate-icon"],
          symbol: ""
        }

        if( q != g_min_q ){ico.clsMiddle.push('up')}
        if( q != g_max_q ){ico.clsMiddle.push('down')}

        if( gate.args.includes(q) ){
          ico.symbol = gateType.kind == "swap"
            ? "×" : `\\(${gateType.symbol}\\)`
        } else if( gate.cargs.includes(q) ){
          ico.clsInner.push("control-dot")
        } else {
          ico.clsInner.push("empty")
        }
        column[q] = ico
      }
    }
    colFinished()
    MathJax.typeset()
    
    $('.state').remove()
    simulate().then(function(states){
      $('.states-error').addClass('hidden')
      $('.states').removeClass('hidden')
      for (let i = 0; i < states.length; i++) {
        const s = states[i];
        let li = percent(s.likelihood, 2)
        let col = fadeAt(s.likelihood, 0.05, 0.3) * 100
        s.el = $(`
          <li class='state'>
            <span class='state-ket'>
              \\(\\ket{${s.state}}\\):
            </span>
            <div class='bar'>
              <div class='fill'
                  style="width: ${li}; color: rgba(0,0,0,${col}%)">
                ${li}
              </div>
            </div>
          </li>`)
          .appendTo('#states')
      }
      MathJax.typeset()
    }).catch(function(a){
      $('.states-error').removeClass('hidden')
      $('.states').addClass('hidden')
      console.log(a)
    })
  }

  $('#refresh').click(refreshGates)

  /*
   * SIMULATION
   */

  const ENDPOINT = "http://localhost:12345"

  simulate = function(){
    const q = qubits.length
    return new Promise(function(fThen, fError){
      $.post(ENDPOINT + "/simulate", {circuit: as_file()})
      .catch(fError)
      .done(function(data){
        const lines = data.split("\n");
        let states = []
        for (let i = 0; i < lines.length; i++) {
          const line = lines[i];
          if( line && !isNaN(line) ){
            let x = states.length
            states.push({
              state: ("0".repeat(q) + x.toString(2)).slice(-q),
              likelihood: Number(line)
            })
          }
        }
        fThen(states)
      })
    })
  }


  /* 
   * FILE FORMAT AND OPERATIONS 
   */


  as_file = function(){
    emitStates = true;

    str = `qubits,${qubits.length}\n`
    if(conf.shots != 1024) str += `shots,${conf.shots}\n`
    if(conf.noise) str += `noise,${conf.noise}\n`
    if(conf.isSparse) str += `sparse\n`
    if(!conf.doGroup) str += `nogroup\n`
    if(conf.emitStates) str += `states\n`
    
    for (let i = 0; i < circuit_gates.length; i++) {
      const gate = circuit_gates[i];
      const id = "c".repeat(gate.cargs.length) + gate.gate
      str += `${id},${gate.all_args.join(",")}\n`
    }
    return str
  }
  let download = function(data, filename, type) {
    let file = new Blob([data], {type: type});
    if (window.navigator.msSaveOrOpenBlob) // IE10+
        window.navigator.msSaveOrOpenBlob(file, filename);
    else { // Others
      let a = document.createElement("a"),
              url = URL.createObjectURL(file);
      a.href = url;
      a.download = filename;
      document.body.appendChild(a);
      a.click();
      setTimeout(function() {
          document.body.removeChild(a);
          window.URL.revokeObjectURL(url);  
      }, 0); 
    }
  }
  
  let filename = "circuit.in"
  $('#btn-save').click(function(){
    download(as_file(), filename, "text/plaintext")
  })

  const GATE_ALIAS = {
    "nop": "id", "hadamard": "h",
    "not": "x",
    "cnot": "cx",
    "tdg": "tinv", "vdg": "vinv",
    "ccnot": "ccx", "toffoli": "ccx", 
  }
  
  let loadingError = false;
  
  $('#btn-load').change(function(){
    let file = $('#btn-load')[0].files[0]
    filename = file.name;

    /* Alas, Safari doesn't support File API */
    let fr = new FileReader()
    fr.readAsText(file)
    loadingError = false

    $('.disp-load-name').removeClass('error')
    $('.disp-load-error').text("")

    function error(lineno, msg){
      msg = `Error in line ${lineno}: ${msg}`
      $('.disp-load-error').text(msg)
      $('.disp-load-name').addClass('error').text("file failed to load!")
      console.log(msg)
      loadingError = true
    }
    
    let f_gates = [];
    let f_conf = conf_default();

    const FILE_LOAD_TIMEOUT = 200;
    setTimeout(function(){
      lines = fr.result.split(/\r\n|\r|\n/)
      for (let l = 0; l < lines.length; l++) {
        const line = lines[l];
        if( !line || line.startsWith(`//`)){
          continue; // strip comments
        }
        let args = line.split(",")
        let name = args.shift().toLowerCase()
        name = GATE_ALIAS[name] || name
        
        function numHeader(cond){
          let token = args.shift()
          if( token == undefined ){
            error(l, `'{desc}' requires a value`)
          }
          let numval = Number(token)
          if( cond(numval) ){
            f_conf[name] = numval
          } else {
            error(l, `'${name}' cannot be ${token}`)
          }
        }
        
        switch( name ){
          case "qubits": numHeader(
            n => Number.isInteger(n) && n > 0
          ); break;
          case "shots": numHeader(
            n => Number.isInteger(n) && n > 0
          ); break;
          case "noise": numHeader(
            n => 0 <= n && n <= 1
          ); break;
          case "sparse": f_conf.isSparse = true; break;
          case "nogroup": f_conf.doGroup = false; break;
          case "states": f_conf.emitStates = true; break;

          default:
            for (let i = 0; i < args.length; i++) {
              const a = args[i];
              if( isNaN(a) ){
                return error(l, `argument "${a}" not a number`)
              }
              args[i] = Number(a)
            }
            let cargs = []
            while (name.startsWith("c")) {
              cargs.push(args.shift())
              name = name.slice(1)
            }
            let gate = GATES[name];
            if( !gate ){
              return error(l, `unknown operation "${name}"`)
            }
            if( args.length < gate.arity ){
              return error(l, `not enough args to ${name} (need ${gate.arity})`)
            } else {
              f_gates.push({
                gate: name,
                args: args,
                cargs: cargs,
                all_args: cargs.concat(args)
              })
            }
            break;
        }
      }
      /* end for loop */
      if( !loadingError ){
        for (let i = 0; i < qubits.length; i++) {
          qubits[i].el.remove()
        }
        qubits = []
        for (let i = 0; i < f_conf.qubits; i++) {
          addQubit()
        }
  
        circuit_gates = f_gates
        conf = f_conf
        $('#check-sparse').prop('checked', conf.isSparse)
        $('#check-group').prop('checked', conf.doGroup)
        $('#slider-shots').val(Math.floor(Math.log2(conf.shots)))
        $('#slider-noise').val(conf.noise * NOISE_SIZE)
  
        $('.disp-load-name').html(`<code>${file.name}</code>`)

        refreshConf()
        refreshSelector()
        refreshGates()
      }
    }, FILE_LOAD_TIMEOUT)
  })
})
