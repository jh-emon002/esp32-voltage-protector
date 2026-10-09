# FSM diagram (to be added)

Please add the actual FSM diagram image to this folder as `fsm_diagram.png` or `fsm_diagram.jpg`.

The states implemented in the firmware are:

- INIT: start-up check
- NORMAL: no active fault, green LED ON
- TRIPPED: threshold exceeded, fault latched, load indicator OFF
- RESET_READY: both readings under recovery margins for at least 1 second; awaiting manual/web reset

Important transitions: INIT -> NORMAL or TRIPPED, NORMAL -> TRIPPED, TRIPPED -> RESET_READY, RESET_READY -> TRIPPED if unsafe, RESET_READY -> NORMAL upon valid reset.

To place the diagram in the one-page report, open `../project_files/design_document_editable.docx`, click the empty FSM box, insert the image, and export to PDF as `../design_document.pdf`.
